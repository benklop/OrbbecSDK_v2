// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "FerretDevice.hpp"
#include "FilterFactory.hpp"
#include "SourcePortInfo.hpp"
#include "logger/Logger.hpp"

#include <algorithm>

namespace libobsensor {

namespace {

SourcePortInfoList::const_iterator findFerretColorPort(const SourcePortInfoList &ports) {
    auto isUvc = [](const std::shared_ptr<const SourcePortInfo> &p) { return p->portType == SOURCE_PORT_USB_UVC; };
    for(uint8_t idx: { uint8_t(4), uint8_t(5) }) {
        auto it = std::find_if(ports.begin(), ports.end(), [&](const std::shared_ptr<const SourcePortInfo> &p) {
            return isUvc(p) && std::dynamic_pointer_cast<const USBSourcePortInfo>(p)->infIndex == idx;
        });
        if(it != ports.end()) {
            return it;
        }
    }
    return std::find_if(ports.begin(), ports.end(), [&](const std::shared_ptr<const SourcePortInfo> &p) {
        if(!isUvc(p)) {
            return false;
        }
        const auto &n = std::dynamic_pointer_cast<const USBSourcePortInfo>(p)->infName;
        return n.find("RGB") != std::string::npos;
    });
}

}  // namespace

FerretDevice::FerretDevice(const std::shared_ptr<const IDeviceEnumInfo> &info) : G2Device(info) {
    // G2Device::G2Device() has already called init() and checkAndStartHeartbeat().
    // Frame processor components are registered lazily in init(); they are not
    // instantiated until streaming starts, so extensionPid_ is set before any
    // closed extension calls ob_device_info_get_pid().

    // CR-Scan Ferret (USB PID 0x069c) is a Gemini 2 OEM.  Prebuilt extensions
    // under extensions/ (frame processor, priv filters, firmware updater) have
    // no source and must not be binary-patched (see extensions/license.txt).
    // extensionPid_ makes ob_device_info_get_pid() return 0x0670 for those libs
    // only; pid_ stays 0x069c for enumeration, logging, and open-source paths.
    deviceInfo_->extensionPid_ = 0x0670;  // Gemini 2 canonical PID

    if(!isComponentExists(OB_DEV_COMPONENT_COLOR_SENSOR)) {
        const auto &ports = enumInfo_->getSourcePortInfoList();
        auto        it    = findFerretColorPort(ports);
        if(it != ports.end()) {
            registerColorSensorFromPort(*it);
        } else {
            LOG_WARN("Ferret: RGB UVC port not found among {} enumerated interfaces", ports.size());
        }
    }

    // UVC backend and stream defaults: OrbbecSDKConfig.xml <CR-ScanFerret> (must
    // exist before G2Device::fixSensorList opens ports; see device name from firmware).

    // Future Ferret-specific post-init work:
    //   • Load algo.obconfig blobs and push them to the frame processor via
    //     PrivFilterCppWrapper::setConfigData() if needed for depth quality.
    //   • Override depth work-mode defaults if the Ferret firmware exposes a
    //     different mode set than the stock Gemini 2.
}

std::vector<std::shared_ptr<IFilter>> FerretDevice::createRecommendedPostProcessingFilters(OBSensorType type) {
    // Delegate to G2Device for now; override with Ferret-tuned defaults later.
    return G2Device::createRecommendedPostProcessingFilters(type);
}

}  // namespace libobsensor
