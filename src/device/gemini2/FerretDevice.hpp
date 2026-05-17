// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.
//
// FerretDevice — Creality CR-Scan Ferret (OEM Orbbec Gemini 2, PID 0x069c)
//
// Thin subclass of G2Device that provides:
//   • extensionPid_ = 0x0670 so closed extensions recognise the device via
//     ob_device_info_get_pid() without modifying prebuilt .so files.
//   • OrbbecSDKConfig.xml <CR-ScanFerret> supplies LibUVC + Gemini2-class defaults.
//   • A distinct type for Ferret-specific logic (e.g. algo.obconfig delivery).
//   • An override hook for createRecommendedPostProcessingFilters().
//
// Construction notes
// ------------------
// G2Device::G2Device() calls init() (private, non-virtual at the call-site)
// before FerretDevice's constructor body runs.  Virtual dispatch is suppressed
// during base-class construction per the C++ standard, so init() always
// executes G2Device's implementation.  Ferret-specific post-init work should
// be placed directly in the FerretDevice constructor body.

#pragma once
#include "G2Device.hpp"

namespace libobsensor {

class FerretDevice : public G2Device {
public:
    FerretDevice(const std::shared_ptr<const IDeviceEnumInfo> &info);
    virtual ~FerretDevice() noexcept override = default;

    std::vector<std::shared_ptr<IFilter>> createRecommendedPostProcessingFilters(OBSensorType type) override;
};

}  // namespace libobsensor
