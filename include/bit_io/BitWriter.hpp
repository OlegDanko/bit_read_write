#ifndef BIT_IO_BITWRITER_HPP
#define BIT_IO_BITWRITER_HPP

#include "BitIOBase.hpp"
#include <vector>
#include <cstddef>

namespace bit_io {

class BitWriter : private BitIOBase {
    std::vector<u8>& buf;
    size_t bytes_offset = 0;
    u8 bits_offset = 0;

    static void write_byte(u8* buf, u8 byte, u8 offset, u8 size = 8) {
        u16 mask = ~(size_to_fill_mask.at(size) << offset);
        as_u16(buf) &= mask;
        u16 b16 = byte & size_to_fill_mask.at(size);
        b16 <<= offset;
        as_u16(buf) |= b16;
    }

    static void write_byte_single(u8* buf, u8 byte, u8 offset, u8 size = 8) { // unsafe: offset+size <= 8 or death
        u8 mask = ~(size_to_fill_mask.at(size) << offset);
        *buf &= mask;
        u8 b8 = byte & size_to_fill_mask.at(size);
        b8 <<= offset;
        *buf |= b8;
    }

public:
    BitWriter(std::vector<u8>& bit_buffer) : buf(bit_buffer) {}

    template<size_t B, typename T>
    bool write(const T& t) {
        static_assert(B != 0, "Can't write 0 bits");
        static_assert(sizeof(T)*8 >= B, "Number of bits is higher than size of the object");

        //check if there's enough space
        auto bytes_left = buf.size() - bytes_offset;
        auto bits_req = B + bits_offset;
        if(bytes_left * 8 < bits_req) {
            auto bytes_req = bits_req / 8 - bytes_left + 1;
            for(size_t i = 0; i < bytes_req; i++) buf.push_back(0);
        }

        const auto& arr = as_arr(&t);
        auto bits = B;
        auto byte_num = 0;

        while(bits > 8) {
            //write byte by byte
            write_byte(&buf.at(bytes_offset), arr.at(byte_num), bits_offset);
            byte_num++;
            bytes_offset++;
            bits -= 8;
        }
        //write remaining bits
        buf.size() - bytes_offset == 1
                ? write_byte_single(&buf.at(bytes_offset), arr.at(byte_num), bits_offset, bits)
                : write_byte(&buf.at(bytes_offset), arr.at(byte_num), bits_offset, bits);
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
