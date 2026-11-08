#pragma once

#include <mmio/chips/Bytes.hpp>

#include <stdint.h>
#include <stddef.h>

namespace MMIO::Chips {

    class I2CChip
    {
    public:
        // Write txLen bytes to the device at addr7, then (if rxLen > 0) issue a repeated start
        // and read rxLen bytes back. A plain write is rxLen == 0.
        //
        // addr7 is the 7-bit address, NOT shifted left to include the R/W bit. This matches
        // Linux I2C_RDWR and Zephyr i2c_write_read; HALs that want the 8-bit form (e.g. STM32
        // HAL) should shift it inside the callback.
        using TransferFn = bool (*)(void* ctx, uint8_t addr7,
                                    const uint8_t* tx, size_t txLen,
                                    uint8_t* rx, size_t rxLen);

        explicit I2CChip(uint8_t address_)
            : address(address_)
        {}

        void SetTransferFn(TransferFn transferFn_, void* ctx_)
        {
            transferFn = transferFn_;
            ctx = ctx_;
        }

        uint8_t Address() const
        {
            return address;
        }

    protected:
        bool Transfer(const uint8_t* tx, size_t txLen, uint8_t* rx, size_t rxLen)
        {
            return transferFn && transferFn(ctx, address, tx, txLen, rx, rxLen);
        }

        // Raw read of len bytes starting at an 8-bit register address
        bool ReadReg(uint8_t reg, uint8_t* out, size_t len)
        {
            return Transfer(&reg, 1, out, len);
        }

        // Typed register read/write, multi-byte values sent most significant byte first
        // (the common convention for I2C peripherals). Use the raw ReadReg plus
        // FromBytesLSB for little-endian devices.
        template <typename T>
        bool ReadReg(uint8_t reg, T& out)
        {
            uint8_t buf[sizeof(T)];
            if (!ReadReg(reg, buf, sizeof(T))) {
                return false;
            }
            out = FromBytesMSB<T>(buf);
            return true;
        }

        template <typename T>
        bool WriteReg(uint8_t reg, T value)
        {
            // Register address and data must go out in a single transaction
            uint8_t buf[1 + sizeof(T)];
            buf[0] = reg;
            ToBytesMSB(value, buf + 1);
            return Transfer(buf, sizeof(buf), nullptr, 0);
        }

        uint8_t address; // 7-bit, unshifted
        TransferFn transferFn = nullptr;
        void* ctx = nullptr;
    };
}
