#pragma once

#include <stdint.h>
#include <mmio/MMIO.hpp>

using namespace MMIO;

class ParallelChip
{

public:
    enum BusMode
    {
        _4Bit,
        _8Bit
    };

    using TransferFn = bool (*)(void* ctx, bool rs, bool rw, uint8_t data);

    explicit ParallelChip() {};

    void SetTransferFn(TransferFn, void* ctx);

    void SetBusMode(BusMode mode_)
    {
        mode = mode_;
    }

protected:
    bool Transfer(bool rs, bool rw, uint8_t data_)
    {
        if (mode == BusMode::_4Bit) {
            bool success = transferFn != nullptr ? true : false;
            if (!success) {
                return false;
            }

            data = data_;

            success &= transferFn(ctx, rs, rw, data[Nibbles::High]);
            success &= transferFn(ctx, rs, rw, data[Nibbles::Low]);

            return success;
        }
        else if (mode == BusMode::_8Bit) {
            return transferFn && transferFn(ctx, rs, rw, data);
        }

        return false;
    }

    BusMode mode = BusMode::_8Bit;
    void* ctx;
    TransferFn transferFn = nullptr;

private:

    BitField<uint8_t> data;
    struct Nibbles
    {
        static constexpr Field<ReadOnly, 7, 4> High{};
        static constexpr Field<ReadOnly, 3, 0> Low{};
    };
};