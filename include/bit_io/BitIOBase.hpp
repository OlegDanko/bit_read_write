#ifndef BIT_IO_BITIOBASE_H
#define BIT_IO_BITIOBASE_H
#include <array>
#include <cstdint>

namespace bit_io {

using u8 = uint8_t;
using u16 = uint16_t;

#define BIT_SIZE(X) (sizeof(X)*8)

class BitIOBase {
public:
    constexpr static std::array<u8, 9> size_to_fill_mask {
        0b00000000,
        0b00000001,
        0b00000011,
        0b00000111,
        0b00001111,
        0b00011111,
        0b00111111,
        0b01111111,
        0b11111111,
    };

    static inline u16& as_u16(u8* b) { //!UNSAFE, use wisely
        return *((u16*)b);
    }

    static inline const u16& as_u16(const u8* b) { //!UNSAFE, use wisely
        return *((u16*)b);
    }

    template<typename T>
    static std::array<u8, sizeof(T)>& as_arr(T *t) {
        return *((std::array<u8, sizeof(T)>*)t);
    }
};

}

#endif
