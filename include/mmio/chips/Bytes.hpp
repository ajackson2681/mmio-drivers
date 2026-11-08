#pragma once

#include <stdint.h>
#include <stddef.h>

// Bus-agnostic helpers for packing register values into byte buffers and back.
// Shared by SPIChip, I2CChip, and any chip driver that needs them.
namespace MMIO::Chips {

    // register value -> bytes, most significant byte first (out[0] = top byte)
    template <typename T>
    void ToBytesMSB(T value, uint8_t* out)
    {
        for (size_t i = 0; i < sizeof(T); ++i) {
            out[i] = static_cast<uint8_t>(value >> (8 * (sizeof(T) - 1 - i)));
        }
    }

    // register value -> bytes, least significant byte first (out[0] = bottom byte)
    template <typename T>
    void ToBytesLSB(T value, uint8_t* out)
    {
        for (size_t i = 0; i < sizeof(T); ++i) {
            out[i] = static_cast<uint8_t>(value >> (8 * i));
        }
    }

    // bytes -> register value, the reverse of the two above
    template <typename T>
    T FromBytesMSB(const uint8_t* in)
    {
        T value = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            value = static_cast<T>((value << 8) | in[i]);
        }
        return value;
    }

    template <typename T>
    T FromBytesLSB(const uint8_t* in)
    {
        T value = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            value |= static_cast<T>(static_cast<T>(in[i]) << (8 * i));
        }
        return value;
    }
}
