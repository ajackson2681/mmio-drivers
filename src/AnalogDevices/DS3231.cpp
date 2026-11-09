#include <mmio/chips/AnalogDevices/DS3231.hpp>

namespace MMIO::Chips::AnalogDevices::DS3231
{
    DS3231::DS3231() 
        : I2CChip(DefaultAddress)
    {}

    bool DS3231::SetSeconds(uint8_t sec)
    {
        Register<uint8_t>& reg = regs[RegAddrs::Seconds];

        reg[Timing::Ones] = sec % 10;
        reg[Timing::Tens] = sec / 10;

        return WriteReg(RegAddrs::Seconds, reg.Value());
    }

    bool DS3231::SetMinutes(uint8_t min)
    {
        Register<uint8_t>& reg = regs[RegAddrs::Minutes];

        reg[Timing::Ones] = min % 10;
        reg[Timing::Tens] = min / 10;

        return WriteReg(RegAddrs::Minutes, reg.Value());
    }

    bool DS3231::SetHours(uint8_t hours)
    {
        Register<uint8_t>& reg = regs[RegAddrs::Hours];
        TimeFormat tf = (TimeFormat)(uint8_t)reg[Hours::TimeFormat];

        reg[Timing::Ones] = hours % 10;
        reg[Timing::Tens] = hours / 10;

        return WriteReg(RegAddrs::Hours, reg.Value());
    }

    bool DS3231::SetTime(uint8_t sec, uint8_t min, uint8_t hour) 
    {
        bool success = true;

        success &= SetSeconds(sec);
        success &= SetMinutes(min);
        success &= SetHours(hour);

        return success;
    }

    bool DS3231::ReadTime()
    {
        uint8_t read[3]; 
        bool success = ReadReg(0, read, 3);
        
        if (success) {
            regs[RegAddrs::Seconds] = read[0];
            regs[RegAddrs::Minutes] = read[1];
            regs[RegAddrs::Hours]   = read[2];
        }

        return success;
    };

    bool DS3231::SetTimeFormat(TimeFormat format)
    {
        Register<uint8_t>& reg = regs[RegAddrs::Hours];

        reg[Hours::TimeFormat] = format;

        return WriteReg(RegAddrs::Hours, reg.Value());
    }
};