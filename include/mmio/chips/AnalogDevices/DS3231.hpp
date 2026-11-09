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
        LSBTemp,
        NumRegisters
    };

    // used in seconds and minutes. Hours has adifferent
    // tens and twenties format. Ones is still used
    struct Timing
    {
        static constexpr Field<ReadWrite, 3, 0> Ones{};
        static constexpr Field<ReadWrite, 6, 4> Tens{};
    };

    // 
    struct Hours
    {
        static constexpr Field<ReadWrite, 4> Ten{};
        static constexpr Field<ReadWrite, 5> PM{};
        static constexpr Field<ReadWrite, 5> Twenty{};
        static constexpr Field<ReadWrite, 6> TimeFormat{};
    };

    enum TimeFormat
    {
        Hour24,
        Hour12
    };

    class DS3231 : public I2CChip
    {
    public:
        static constexpr uint8_t DefaultAddress = 0x68; // fixed in silicon

        DS3231();

        bool SetSeconds(uint8_t seconds);
        bool SetMinutes(uint8_t minutes);
        bool SetHours(uint8_t hours);
        bool SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours);
        bool SetTimeFormat(TimeFormat format);
        bool ReadTime();

        Register<uint8_t> regs[NumRegisters];
    };
};