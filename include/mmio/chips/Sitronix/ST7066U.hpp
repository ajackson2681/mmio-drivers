#pragma once

#include <mmio/chips/ParallelChip.hpp>
#include <mmio/MMIO.hpp>

namespace MMIO::Chips::Sitronix::ST7066U
{
    static constexpr uint8_t MaxRows = 4;
    static constexpr uint8_t MaxCols = 40;
    static constexpr uint8_t MaxCharsPerChip = 80;


    // static commands that have no modifications. These are always the same
    struct Instructions
    {
        static constexpr uint8_t ClearDisplay   = 0b0000'0001;
        static constexpr uint8_t ReturnHome  = 0b0000'0010;
    };

    struct Shared
    {
        static constexpr Field<ReadWrite, 7, 0> Data{};
        static constexpr Field<ReadWrite, 7, 4> HighNib{};
        static constexpr Field<ReadWrite, 3, 0> LowNib{};
    };

    enum CursorStyle
    {
        Line,
        Block
    };


    namespace EntryModeSet
    {
        enum CursorDir
        {
            Right,
            Left
        };

        enum DispShift
        {
            Off,
            On
        };

        struct EntryModeSet
        {
            // the starting point of the command to be modified. Write this
            // into the bitfield before modifying it.
            static constexpr uint16_t Base = 0b0000'0100;

            static constexpr Field<ReadWrite, 1> CursorDirection{};
            static constexpr Field<ReadWrite, 0> ShiftDisplay{};

        };
    }

    struct DisplayControl
    {
        static constexpr uint16_t Base = 0b0000'1000;
        static constexpr Field<ReadWrite, 2> DisplayOn{};
        static constexpr Field<ReadWrite, 1> CursorOn{};
        static constexpr Field<ReadWrite, 0> CursorBlink{};
    };

    namespace CursorControl
    {
        enum CursorSetting
        {
            ShiftCursorLeft = 0b00,
            ShiftCursorRight = 0b01,
            ShiftDispAndCursorLeft = 0b10,
            ShiftDispAndCursorRight = 0b11
        };

        struct CursorControl
        {
            static constexpr uint16_t Base = 0b0001'0000;
            static constexpr Field<ReadWrite, 3, 2> Setting{};
        };
    }


    namespace FunctionSet
    {

        enum LineSet
        {
            One,
            Two
        };

        enum FontMode
        {
            Mode5x8,
            Mode5x11
        };

        struct FunctionSet
        {
            static constexpr uint16_t Base = 0b0010'0000;

            static constexpr Field<ReadWrite, 4> BusMode{};
            static constexpr Field<ReadWrite, 3> NumLines{};
            static constexpr Field<ReadWrite, 2> FontMode{};
        };
    }

    struct SetCGRAM
    {
        static constexpr uint16_t Base = 0b0100'0000;
        static constexpr Field<ReadWrite, 5, 0> Address{};
    };

    struct SetDDRAM
    {
        static constexpr uint16_t Base = 0b1000'0000;
        static constexpr Field<ReadWrite, 6, 0> Address{};

        /**
         * @brief calculates the offset for a given row/column combination. This
         * calculation gets a little hairy because it's entirely dependent on
         * how many rows/columns there are in the display.
         *
         * For instance, a single ST7066U controller can manipulate up to 80
         * characters.
         *
         * This could mean 80x1 or 40x2 at most. But there can be less characters
         * per line. Such as 16x2, or 16x4, or 20x4.
         *
         * The key takeaway here is that internally, there really isn't any
         * "rows" or "columns". It's basically just contiguous memory starting
         * at 0x00.
         *
         * @param row
         * @param col
         * @return uint8_t
         */
        template <uint8_t DispRows, uint8_t DispCols>
        static uint8_t GetOffset(uint8_t row, uint8_t col)
        {
            if (DispRows * DispCols > MaxCharsPerChip) {
                 uint8_t offset = col;
                 if (row & 1) { // rows 1 & 3
                    offset += 0x40;
                 }

                 return offset;
            }
            else {
                return row * DispRows + col;
            }
        }
    };

    struct Status
    {
        static constexpr uint16_t Base = 0b0000'0000;
        static constexpr Field<ReadWrite, 7> Busy{};
        static constexpr Field<ReadWrite, 6, 0> Address{};
    };

    struct WriteRAM
    {
        static constexpr uint16_t Base = 0b0000'0000;
        static constexpr Field<ReadWrite, 7, 0> Data{};
    };

    struct ReadRAM
    {
        static constexpr uint16_t Base = 0b0000'0000;
        static constexpr Field<ReadWrite, 7, 0> Data{};
    };

    struct ST7066UConfig
    {
        using FontMode = FunctionSet::FontMode;
        using CursorSetting = CursorControl::CursorSetting;
        using CursorDir = EntryModeSet::CursorDir;
        using DispShift = EntryModeSet::DispShift;
        using BusMode = ParallelChip::BusMode;

        BusMode busMode = BusMode::_4Bit;
        FontMode fontMode = FontMode::Mode5x8;
        CursorStyle cursorStyle = CursorStyle::Block;
        bool displayOn = 1;
        CursorDir cursorDir = CursorDir::Right;
        DispShift shiftDisplay = DispShift::Off;
        CursorSetting cursorSetting = CursorSetting::ShiftCursorRight;
    };

    template <uint8_t Rows, uint8_t Cols>
    class ST7066U : public ParallelChip
    {
        using FunctionSet = FunctionSet::FunctionSet;
        using CursorControl = CursorControl::CursorControl;
        using EntryModeSet = EntryModeSet::EntryModeSet;

    public:

        ST7066U(ST7066UConfig& config_)
            : config(config_)
        {
            static_assert(Rows > 0, "ST7066U Must have at least 1 row");
            static_assert(Rows <= MaxRows, "ST7066U Can only support a max of 4 rows");
            static_assert(Cols > 0, "ST7066U Must have at least 1 column");
            static_assert(Cols <= MaxCols, "ST7066U Can only support a max of 40 columns");

            clearDisplay = Instructions::ClearDisplay;
            returnHome = Instructions::ReturnHome;
            entryMode = EntryModeSet::Base;
            dispCtrl = DisplayControl::Base;
            cursorCtrl = CursorControl::Base;
            busCtrl = FunctionSet::Base;
            CGRAMCtrl = SetCGRAM::Base;
            DDRAMCtrl = SetDDRAM::Base;
        }

        ST7066U()
            : ST7066U({})
        {}

        void Initialize()
        {
            entryMode[EntryModeSet::CursorDirection] = config.cursorDir;
            entryMode[EntryModeSet::ShiftDisplay] = config.shiftDisplay;

            dispCtrl[DisplayControl::DisplayOn] = config.displayOn;
            // dispCtrl[DisplayControl::CursorBlink] = config.
            // dispCtrl[DisplayControl::CursorOn] = config.cursorSetting;

            cursorCtrl[CursorControl::Setting] = config.cursorSetting;

            busCtrl[FunctionSet::BusMode] = config.busMode;
            // ensure the parent also gets this information. It informs it of
            // how to use the transfer function
            SetBusMode(config.busMode);

            busCtrl[FunctionSet::FontMode] = config.fontMode;
        }

        /**
        * @brief Clears the screen and moves to 0,0
        */
        bool ClearDisplay()
        {
            return WriteCommand(Instructions::ClearDisplay);
        }

        bool EnableCursor()
        {
            dispCtrl[DisplayControl::CursorOn] = 1;
            return WriteCommand(dispCtrl.Value()); 
        }

        void DisableCursor()
        {
            dispCtrl[DisplayControl::CursorOn] = 0;
            return WriteCommand(dispCtrl.Value());
        }

        void Put(char c)
        {
            return WriteChar(c);
        }

        void SetCursorStyle(CursorStyle style);
        void SetCursorBlink(bool shouldBlink);

        void SetConfig(ST7066UConfig& config_) const { config = config_; }

        /**
         * @brief Set the Cursor Position to a specified row and column.
         * If the row/column passed here is outside the maximum allowed, it will
         * clamp to the max
         *
         * @param row
         * @param col
         */
        void SetCursorPos(uint8_t row, uint8_t col)
        {
            if (row > MaxRows) {
                row = MaxRows - 1;
            }

            if (col > MaxCols) {
                col = MaxCols - 1;
            }

            DDRAMCtrl[SetDDRAM::Address] = SetDDRAM::GetOffset<Rows, Cols>(row, col);

            return WriteCommand(DDRAMCtrl.Value());
        }

    private:
        union
        {
            bool rs : 1;
            bool rw : 1;
        } ControlPins;

        BitField<uint8_t> clearDisplay;
        BitField<uint8_t> returnHome;
        BitField<uint8_t> entryMode;
        BitField<uint8_t> dispCtrl;
        BitField<uint8_t> cursorCtrl;
        BitField<uint8_t> busCtrl;
        BitField<uint8_t> CGRAMCtrl;
        BitField<uint8_t> DDRAMCtrl;

        BitField<uint8_t> character;

        ST7066UConfig& config;

        bool WriteCommand(uint8_t byte)
        {
            return Transfer(false, false, byte);
        }

        bool WriteChar(uint8_t byte)
        {
            return Transfer(true, false, byte);
        }
        
    };
}