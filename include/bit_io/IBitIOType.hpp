#ifndef BIT_IO_I_BIT_IO_TYPE_HPP
#define BIT_IO_I_BIT_IO_TYPE_HPP

#include "BitReader.hpp"
#include "BitWriter.hpp"

namespace bit_io {

struct IWriteable {
    virtual void write(BitWriter&) const = 0;
};

struct IReadable {
    virtual void read(BitReader&) = 0;
};


struct IBitIOType : IWriteable, IReadable {
    virtual size_t bit_size() const = 0;

};

}

#endif
