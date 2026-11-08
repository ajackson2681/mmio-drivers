#pragma once

#include <mmio/chips/I2CChip.hpp>
#include <mmio/MMIO.hpp>

namespace MMIO::Chips::AnalogDevices::DS3231
{
    enum RegAddrs
    {
        Seconds,
        Minutes,
        Hours,
        Day,
        Date,
        MonthCentury,
        Year,
        SecondsAlarm1,
        MinutesAlarm1,
        HoursAlarm1,
        DayAlarm1,
        DateAlarm1,
        MinutesAlarm2,
        HoursAlarm2,
        DayAlarm2,
        DateAlarm2,
        Control,
        ControlStatus,
        AgingOffset,
        MSBTemp,
        LSBTemp
    };

    struct Seconds
    {
        static constexpr Field<ReadWrite, 3, 0> SecondsOnes{};
        static constexpr Field<ReadWrite, 6, 4> SecondsOnes{};
    };
    
    class DS3231 : public I2CChip
    {
    public:
        static constexpr uint8_t DefaultAddress = 0x68; // fixed in silicon

        DS3231();

        Register<uint8_t> regs[0x12];
    };
};