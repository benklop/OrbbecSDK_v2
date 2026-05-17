# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

file (GLOB _cpp_files "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file (GLOB _head_files "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

foreach(_file IN ITEMS ${_cpp_files})
        target_sources(${OB_TARGET_DEVICE} PRIVATE "${_file}")
endforeach()

foreach(_file IN ITEMS ${_head_files})
        target_sources(${OB_TARGET_DEVICE} PRIVATE "${_file}")
endforeach()

# GCC 12+ false positive: the bounds analyser incorrectly reports std::mutex
# construction as overflowing a 32-byte buffer inside stl_construct.h when
# GyroSensor's constructor is inlined.  The actual object size is correct;
# suppress the warning for this TU only.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties(
        "${CMAKE_CURRENT_LIST_DIR}/GyroSensor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/AccelSensor.cpp"
        PROPERTIES COMPILE_OPTIONS "-Wno-array-bounds"
    )
endif()
