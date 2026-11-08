cmake_minimum_required(VERSION 3.21)
project(mmio_drivers LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# --- dependency: MMIO (skipped if the parent project already provides it)
if(NOT TARGET MMIO::MMIO)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/mmio/CMakeLists.txt")
        message(FATAL_ERROR "MMIO submodule missing; run: git submodule update --init --recursive")
    endif()
    add_subdirectory(external/mmio)
endif()

# --- shared base: SPIChip.hpp + MMIO
add_library(mmio_chips_common INTERFACE)
target_include_directories(mmio_chips_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mmio_chips_common INTERFACE MMIO::MMIO)

# --- one block per chip
add_library(mmio_analog_devices_DS3231 STATIC src/AnalogDevices/DS3231.cpp)
add_library(MMIO::Chips::AnalogDevices::DS3231 ALIAS mmio_analog_devices_DS3231)
target_link_libraries(mmio_analog_devices_DS3231 PUBLIC mmio_chips_common)


