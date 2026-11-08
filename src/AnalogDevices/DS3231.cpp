#include <mmio/chips/AnalogDevices/DS3231.hpp>

namespace MMIO::Chips::AnalogDevices::DS3231
{
    DS3231::DS3231() 
        : I2CChip(DefaultAddress)
    {}
};