#ifndef BIT_IO_TYPES_HPP
#define BIT_IO_TYPES_HPP

#include "IBitIOType.hpp"

namespace bit_io {

template<typename T>
constexpr size_t useful_bits(T t) {
    auto l = t;
    size_t s = sizeof(T) * 8;
    T m = 1;
    m <<= s - 1;
    m = ~m;
    while(m != 0) {
        l &= m;
        if(l != t) break;
        m >>= 1;
        --s;
    }
    return s;
}

template<typename T, size_t SX = BIT_SIZE(T)>
struct type_t : public IBitIOType {
    T val;

    type_t() {}

    type_t(T _x)
        : val(_x) {}

    type_t(BitReader& buf) {
        read(buf);
    }

    bool operator==(const type_t& that) const {
        return val == that.val;
    }

    bool operator!=(const type_t& that) const {
        return !operator==(that);
    }

    size_t bit_size() const override {
        return SX;
    }

    void write(BitWriter& buf) const override {
        buf.template write<SX>(val);
    }

    void read(BitReader& buf) override {
        if(!buf.template read<SX>(val)) throw "Packet incomplete\n";
    }

    operator T() const { return val; }
};


using flag = type_t<u8, 1>;

template<typename T, size_t... SIZES>
class vec : public IBitIOType {
    template<size_t... _SIZES>
    class sizes_access {
        template<size_t S>
        static size_t _count() { return 1; }
        template<size_t S, size_t... SS>
        static size_t _count() { return 1 + _count<SS...>(); }

        template<size_t ID, size_t S>
        static size_t _get() {
            static_assert (ID == 0, "Index put of bounds");
            return S;
        }
        template<size_t ID, size_t S, size_t... SS>
        static size_t _get() { return ID == 0 ? S : get<ID - 1, SS...>(); }

        template<size_t S>
        static size_t _sum() { return S; }
        template<size_t S, size_t... SS>
        static size_t _sum() { return S + _count<SS...>(); }
    public:
        static size_t count() { return _count<_SIZES...>(); }
        template<size_t ID>
        static size_t get() { return _get<ID, _SIZES...>(); }
        static size_t sum() { return _sum<_SIZES...>();}
    };
    using sa = sizes_access<SIZES...>;
    std::array<T, sa::count()> vals;
    template<size_t ID = 0>
    void _write(BitWriter& bw) {
        bw.template write<sa::get<ID>()>(vals[ID]);
        if(ID < sa::count())
            _write<ID + 1>(bw);
    }
    template<size_t ID = 0>
    void _read(BitReader& br) {
        br.template read<sa::get<ID>()>(vals[ID]);
        if(ID < sa::count())
            _read<ID + 1>(br);
    }
public:
    vec(BitReader& br) { read(br); }

    size_t bit_size() const override { return sa::sum(); }
    void write(BitWriter& bw) const override { _write(bw); }
    void read(BitReader& br) override { _read(br); }

    template<size_t ID>
    T& get() { return vals[ID]; }
};

template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX>
struct vec2 : public vec<T, SX, SY> {
    using base = vec<T, SX, SY>;
    T& x() { return base::template get<0>(); }
    T& y() { return base::template get<1>(); }
};


template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX, size_t SZ = SX>
struct vec3 : public vec2<T, SX, SY> {
    using base = vec2<T, SX, SY>;
    T& z() { return base::template get<2>(); }
};

template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX, size_t SZ = SX, size_t SA = SX>
struct vec4 : public vec3<T, SX, SY, SZ> {
    using base = vec3<T, SX, SY, SZ>;
    T& a() { return base::template get<3>(); }
};

}

#endif
