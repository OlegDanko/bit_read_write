#ifndef BIT_IO_BITREADER_HPP
#define BIT_IO_BITREADER_HPP

#include "BitIOBase.hpp"
#include <vector>
#include <cstddef>

namespace bit_io {

class BitReader : private BitIOBase {
    const buffer_t &buf;
    size_t bytes_offset = 0;
    u8 bits_offset = 0;

    static void read_byte(const u8* buf, u8& byte, u8 offset, u8 size = 8) {
        u16 u16 = as_u16(buf) >> offset;
        u16 &= size_to_fill_mask.at(size);
        byte = u16;
    }

    static void read_byte_single(const u8* buf, u8& byte, u8 offset, u8 size = 8) { // unsafe: offset+size <= 8 or death
        u8 u8 = *buf >> offset;
        u8 &= size_to_fill_mask.at(size);
        byte = u8;
    }


public:
    BitReader(const buffer_t &bit_buffer) : buf(bit_buffer) {}
    template<size_t B, typename T>
    bool read(T& t) {
        static_assert(B != 0, "Can't read 0 bits");
        static_assert(sizeof(T)*8 >= B, "Number of bits is higher than size of the object");

        if((buf.size() - bytes_offset) * 8 < B + bits_offset) return false;

        auto& arr = as_arr(&t);
        auto bits = B;
        auto byte_num = 0;

        while(bits > 8) {
            //read byte by byte
            read_byte(&buf.at(bytes_offset), arr.at(byte_num), bits_offset);
            byte_num++;
            bytes_offset++;
            bits -= 8;
        }
        //read remaining bits
        buf.size() - bytes_offset == 1
                ? read_byte_single(&buf.at(bytes_offset), arr.at(byte_num), bits_offset, bits)
                : read_byte(&buf.at(bytes_offset), arr.at(byte_num), bits_offset, bits);
        bits_offset += bits;
        if(bits_offset >= 8) { // bits_offset + bits can't ever be 16 or higher at this point
            bits_offset %= 8;
            bytes_offset++;
        }
        return true;
    }
};

}

#endif
