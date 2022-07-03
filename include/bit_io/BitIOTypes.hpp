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


template<typename T, size_t SIZE = BIT_SIZE(T)>
struct type_t : public IBitIOType {
    T val{0};

    type_t() {}

    type_t(T v)
        : val(v) {}

    type_t(const type_t& t) : val(t.val) {}

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
        return SIZE;
    }

    void write(BitWriter& buf) const override {
        buf.template write<SIZE>(val);
    }

    void read(BitReader& buf) override {
        if(!buf.template read<SIZE>(val)) throw "Packet incomplete\n";
    }

    operator T() const { return val; }
};

using flag = type_t<u8, 1>;

template<typename...>
class many;

template<typename T>
void write_many(BitWriter& bw, T& t) { t.write(bw); }
template<typename T>
void read_many(BitReader& br, T& t) { t.read(br);}

template<typename T, typename ... Ts>
void write_many(BitWriter& bw, T& t, Ts&... ts) { t.write(bw); write_many(bw, ts...); }
template<typename T, typename ... Ts>
void read_many(BitReader& br, T& t, Ts&... ts) { t.read(br); read_many(br, ts...); }

template<size_t...>
struct sizes;
template<size_t S>
struct sizes<S> {
    static constexpr size_t count() { return 1; }
    static constexpr size_t sum() { return S; }
    template<size_t I>
    static constexpr size_t get() {
        static_assert(I == 0, "Out of bound size requested");
        return S;
    }
};
template<size_t S, size_t ... SS>
struct sizes<S, SS...> {
    static constexpr size_t count() { return 1 + sizes<SS...>::count(); }
    static constexpr size_t sum() { return S + sizes<SS...>::sum(); }
    template<size_t I>
    static constexpr size_t get() {
        if constexpr (I == 0) return S;
        else return sizes<SS...>::template get<I - 1>();
    }
};

template<typename T, size_t... SIZES>
class vec : public IBitIOType {
    using sa = sizes<SIZES...>;
public:
    std::array<T, sa::count()> vals{0};

private:
    template<size_t I , typename ... Args>
    void _set_vals(Args ... args);

    template<size_t I , typename A>
    void _set_vals(A a) {
        vals[I] = a;
    }

    template<size_t I = 0, typename A, typename ... Args>
    void _set_vals(A a, Args ... args) {
        if(I == vals.size()) return;
        vals[I] = a;
        _set_vals<I + 1>(args...);
    }

    template<size_t ID = 0>
    void _write(BitWriter& bw) const {
        bw.template write<sa::template get<ID>()>(vals[ID]);
        if constexpr (sa::count() <= ID + 1) return;
        else _write<ID + 1>(bw);
    }
    template<size_t ID = 0>
    void _read(BitReader& br) {
        br.template read<sa::template get<ID>()>(vals[ID]);
        if constexpr (sa::count() <= ID + 1) return;
        else _read<ID + 1>(br);
    }
public:
    template<typename ... Args>
    vec(Args ... args) { _set_vals(args...); }
    vec(BitReader& br) {
        for(auto& v : vals) v = 0;
        read(br);
    }

    size_t bit_size() const override { return sa::sum(); }
    void write(BitWriter& bw) const override { _write(bw); }
    void read(BitReader& br) override { _read(br); }

    template<size_t ID>
    T& get() { return vals[ID]; }
};

template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX>
struct vec2 : IBitIOType {
    type_t<T, SX> x;
    type_t<T, SY> y;
    void write(BitWriter& bw) const override { write_many(bw, x, y); }
    void read(BitReader& bw) override { read_many(bw, x, y); }
    size_t bit_size() const override { return sizes<SX, SY>::sum(); }
    vec2(T x = 0, T y = 0, T z = 0) : x(x), y(y) {}
    vec2(decltype(x) x, decltype(y) y) : x(x), y(y) {}
    vec2(BitReader& br) { read(br); }
};

template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX, size_t SZ = SX>
struct vec3 : IBitIOType {
    type_t<T, SX> x;
    type_t<T, SY> y;
    type_t<T, SZ> z;
    void write(BitWriter& bw) const override { write_many(bw, x, y, z); }
    void read(BitReader& bw) override { read_many(bw, x, y, z); }
    size_t bit_size() const override { return sizes<SX, SY, SZ>::sum(); }
    vec3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
    vec3(decltype(x) x, decltype(y) y, decltype(z) z) : x(x), y(y), z(z) {}
    vec3(BitReader& br) { read(br); }
};

template<typename T, size_t SX = BIT_SIZE(T), size_t SY = SX, size_t SZ = SX, size_t SA = SX>
struct vec4 : public IBitIOType {
    type_t<T, SX> x;
    type_t<T, SY> y;
    type_t<T, SZ> z;
    type_t<T, SZ> a;
    void write(BitWriter& bw) const override { write_many(bw, x, y, z, a); }
    void read(BitReader& bw) override { read_many(bw, x, y, z, a); }
    size_t bit_size() const override { return sizes<SX, SY, SZ>::sum(); }
    vec4(T x = 0, T y = 0, T z = 0, T a = 0) : x(x), y(y), z(z), a(a) {}
    vec4(decltype(x) x, decltype(y) y, decltype(z) z, decltype(a) a) : x(x), y(y), z(z), a(a) {}
    vec4(BitReader& br) { read(br); }
};

}

#endif
