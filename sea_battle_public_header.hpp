#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>


struct pos {
    using T = unsigned;
    constexpr static unsigned dim = 2;
    T x;
    T y;

    pos() = default;
    constexpr pos(T i, T j) : x(i), y(j) {}
    pos(const pos&) = default;
    pos(pos&&) = default;
    pos& operator=(const pos&) = default;
    pos& operator=(pos&&) = default;

    pos operator+(const pos& other) {
        pos res;
        res.x = x + other.x;
        res.y = y + other.y;
        return res;
    }
};


namespace Ship {
    using id_t = uint8_t; // nonzero

    enum class Type : uint8_t {
        Submarine = 1, 
        Boat = 2, 
        Destroyer = 3, 
        Carrier = 4
    };
    struct Description {
        Type type;
        unsigned hp;
    };
    enum class Orientation : uint8_t {
        Xpp, Ypp, Xmm, Ymm
    };
    pos get_increment(Orientation o) {
        if (o == Orientation::Xpp) { return pos(1,0); }
        if (o == Orientation::Xmm) { return pos(-1, 0); }
        if (o == Orientation::Ypp) { return pos(0, 1); }
        if (o == Orientation::Ymm) { return pos(0, -1); }
        __builtin_unreachable();
    }
    enum class HitStatus {
        Malformed,
        Repeated,
        Missed,
        Damaged, 
        Sunk
    };
    struct AttackResult {
        HitStatus status;
        Type type;
    };
};

