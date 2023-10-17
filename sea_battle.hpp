#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <string>

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
        Submarine = 0, 
        Boat = 1, 
        Destroyer = 2, 
        Carrier = 3
    };

    uint8_t get_ship_length(Type t) {
        return static_cast<uint8_t>(t) + 1;
    }
    constexpr static std::array<Type, 4> ship_types = {Type::Submarine, Type::Boat, Type::Destroyer, Type::Carrier};
    struct Description {
        Type type;
        unsigned hp;
    };

    enum class Orientation : uint8_t {
        Xpp, Ypp, Xmm, Ymm
    };
    constexpr static std::array<Orientation, 4> orientations = {Orientation::Xmm, Orientation::Xpp, Orientation::Ymm, Orientation::Ypp};
    
    constexpr pos get_increment(Orientation o) noexcept {
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



struct cell {
    Ship::id_t val = 0; 
    static constexpr uint8_t hit_mask = 1 << (sizeof(Ship::id_t)*8 - 1);
    static constexpr uint8_t id_mask = (Ship::id_t)(-1) - hit_mask;
    static_assert((id_mask | hit_mask) == (Ship::id_t)(-1));
    static_assert((id_mask & hit_mask) == 0);

    cell() = default;
    cell(Ship::id_t v) : val(v) {};
    cell(const cell&) = default;
    cell& operator=(const cell&) = default;
    cell(cell&&) noexcept = default;
    cell& operator=(cell&) noexcept = default;
    void zero() { val = 0; }
    void set_id(Ship::id_t ship_id) { val |= (ship_id & id_mask); };
    Ship::id_t get_id() { return val & id_mask; }
    void hit() { val |= hit_mask; }
    bool get_hit() { return val & hit_mask; }
};

struct Field {
public:
    constexpr static std::array<pos, 8> neigh_shifts = {
        pos(-1,0), pos(+1, 0), pos(0, -1), pos(0, +1),
        pos(-1,-1), pos(+1, -1), pos(-1, +1), pos(+1, +1)
    };

    constexpr static unsigned maxdeg = neigh_shifts.size();
    const size_t shapex = 0;
    const size_t shapey = 0;
    bool is_allowed(pos p) const noexcept { return p.x < shapex && p.y < shapey; }

    struct Neighs {
        unsigned neigh_cnt;
        std::array<pos, maxdeg> neighs;
        auto begin() { return neighs.begin(); }
        auto end() { return neighs.begin()+neigh_cnt; }
    };

    // Neighs neighs(pos p) noexcept {
    //     Neighs res;
    //     std::array<pos, 4> shifts = {pos(-1,0), pos(+1, 0), pos(0, -1), pos(0, +1)};
    //     for (auto& shift : shifts) {
    //         if (is_allowed(p + shift)) {
    //             res.neighs[res.neigh_cnt++] = p + shift;
    //         }
    //     }
    //     return res;
    // }

};


template <template<typename> class T, unsigned ... I>
struct Grid : public Field {
protected:
    T<cell> underlying;
public:
    unsigned ship_count = 0;
    std::array<unsigned, sizeof...(I)> ship_counts;
    std::array<Ship::Description, (I + ...) + 1> ship_hps;
public:
    constexpr static unsigned max_ship_count = (I + ...);
    constexpr static std::array<unsigned, sizeof...(I)> max_ship_counts = std::array<unsigned, sizeof...(I)>({I...});
    static_assert(max_ship_count < cell::hit_mask);
    template <class U>
    Grid(size_t shapex_, size_t shapey_,  U u = 0) : Field({shapex_, shapey_}), underlying(shapex_ * shapey_, u), ship_count(0), ship_counts({}) {};
    Grid() = default;
    Grid(Grid&&) = default;
    Grid(const Grid&) = default;
    Grid& operator=(const Grid&) = default;
    void clear() {
        for (auto& cell : underlying) {
            cell.zero();
        }
        for (auto& x : ship_counts) {
            x = 0;
        }
        ship_count = 0;
    }

    auto& operator[](pos p) noexcept { return underlying[p.x * shapey + p.y];}
    const auto operator[](pos p) const noexcept { return underlying[p.x * shapey + p.y]; }

    std::string ascii_print() {
        std::string res;
        res.reserve((shapex + 1) * shapey);
        std::array<char, 4> ships = {'S', 'D', 'B', 'C'};
        std::array<char, 4> dead_ships = {'s', 'd', 'b', 'c'};
        for (unsigned i = 0; i < shapex; i++) {
            for (unsigned j = 0; j < shapey; j++) {
                pos p {i,j};
                auto& cell = this->operator[](p);
                char ch = 'N';
                if (cell.get_id() == 0) {
                    if (cell.get_hit() == 0) {
                        ch = ' ';
                    } else {
                        ch = '#';
                    }
                } else {
                    int type = static_cast<unsigned>(ship_hps[cell.get_id()].type);
                    if (type > 4) {
                        throw 1;
                    }
                    if (cell.get_hit() == 0) {
                        ch = ships[type];
                    } else {
                        ch = dead_ships[type];
                    }
                }
                res.push_back(ch);
            }
            res.push_back('\n');
        }
        return res;
    }
};

template <class Grid>
struct PlacementGrid {
private:
    Grid& grid;
public:
    PlacementGrid(Grid& g) : grid(g) {};
    void clear() { grid.clear(); }
    auto max_ship_counts() { return grid.max_ship_counts; }
    auto ship_counts() { return grid.ship_counts; }
    auto shapex() { return grid.shapex; }
    auto shapey() { return grid.shapey; }
    bool is_full() {
        return grid.max_ship_count == grid.ship_count;
    }
    
    bool is_placement_allowed(pos p) {
        if (!grid.is_allowed(p)) {
            return false;
        }
        if (grid[p].get_id() != 0) {
            return false;
        }
        for (auto& n : grid.neigh_shifts) {
            auto neigh = p + n;
            if (!grid.is_allowed(neigh)) {
                return false;
            }
            if (grid[neigh].get_id() != 0) {
                return false;
            }
        }
        return true;
    }
    // Id or zero if error
    Ship::id_t place_ship(pos p, Ship::Type t, Ship::Orientation o) {
        int type = static_cast<int>(t);
        if (grid.ship_counts[type] >= grid.max_ship_counts[type]) {
            return 0;
        }
        pos increment = Ship::get_increment(o);
        unsigned len = Ship::get_ship_length(t);
        unsigned i = 0;
        pos cur = p;
        for (; i < len; i++) {
            if (!is_placement_allowed(cur)) {
                return 0;
            }
            cur = cur + increment;
        }
        Ship::id_t id = ++grid.ship_count;
        grid.ship_counts[type]++; 
        i = 0;
        cur = p;
        for (; i < len; i++) {
            grid[cur].set_id(id);
            cur = cur + increment;
        }
        Ship::Description d = {t, len};
        grid.ship_hps[id] = d;
        return id;
    }

};
template <class Grid>
struct ShootingGrid {
private:
   unsigned shot_count;
   Grid& grid;
public:
    ShootingGrid(Grid& g) : grid(g) {};
    unsigned get_shot_count() {
        return shot_count;
    }
    bool is_allowed(pos p) { return grid.is_allowed(p); }
    bool all_shot() { return grid.ship_count == 0; }
    auto max_ship_counts() { return grid.max_ship_counts; }
    auto ship_counts() { return grid.ship_counts; }
    auto shapex() { return grid.shapex; }
    auto shapey() { return grid.shapey; }
    bool was_hit(pos p) { return grid[p].get_hit() != 0; }
    bool was_damaged(pos p) { return grid[p].get_hit() != 0 && grid[p].get_id() != 0; }


    Ship::AttackResult attack(pos p) {
        if (!grid.is_allowed(p)) {
            return {Ship::HitStatus::Malformed, Ship::Type::Submarine};
        }
        if (grid[p].get_hit()) {
            return {Ship::HitStatus::Repeated, Ship::Type::Submarine};
        }
        grid[p].hit();
        shot_count ++;
        if (grid[p].get_id() == 0) {
            return {Ship::HitStatus::Missed, Ship::Type::Submarine};
        }

        Ship::id_t id = grid[p].get_id();
        auto left_hps = --grid.ship_hps[id].hp;
        auto type = grid.ship_hps[id].type;
        if (left_hps == 0) {
            grid.ship_count--;
            return {Ship::HitStatus::Sunk, type};
        } else {
            return {Ship::HitStatus::Damaged, type};
        }
    }
}; 