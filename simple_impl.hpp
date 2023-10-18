#include "sea_battle.hpp"
#include <iostream>
#include <algorithm>


namespace placement_generators {


template <class Placement>
struct Simple {
    Placement* field = nullptr;
    std::random_device random_device;
    std::mt19937 randgen;


    Simple() : field(nullptr), randgen(random_device()){};
    Simple(Placement& f) : field(&f), randgen(random_device()) {}

    void set(Placement& f) { field = &f; }

    void generate() {
        restart:
        auto max_ship_counts = field->max_ship_counts();
        int iters = 0;
        std::uniform_int_distribution<int> rand_pos_x(0, field->shapex() -1);
        std::uniform_int_distribution<int> rand_pos_y(0, field->shapey() -1);
        std::uniform_int_distribution<int> rand_orient(0, 3);


        // place the ships somwhat randomly
        for (int i = 3; i >= 0; i--) {
            for (unsigned c = 0; c < max_ship_counts[i];) {
                iters++;
                pos p;
                p.x = rand_pos_x(randgen);
                p.y = rand_pos_y(randgen);
                auto o = Ship::orientation_list[rand_orient(randgen)];
                // auto o = Ship::orientation_list[0];
                auto t = Ship::ship_types_list[i];
                if (field->place_ship(p, t, o) == 0) {
                    // Admit failure if it took to long
                    if (iters >= 1000000) {
                        std::cout << "Failure";
                        return;
                    }
                    // restart if it seems to take long
                    if (iters >= 200) {
                        field->clear();
                        goto restart;
                    }
                    continue;
                } else {
                    c++;
                }
            }
        }
    }
};
};

namespace permutations {
    std::random_device random_device;
    std::mt19937 randgen(random_device());

    struct IsShufflable {};

    struct Permutation : std::vector<uint>, IsShufflable {
    public:
        void Id() {
            for (uint i = 0; i < size(); i++) {
                this->operator[](i) = i;
            }
        }
        void Shuffle() {
            std::shuffle(this->begin(), this->end(), randgen);
        }
    };

    template <class T>
    struct Shuffled : T, IsShufflable {
    private:
        using value_type = typename T::value_type;
        Permutation sigma;
        void InitPermutation() {
            sigma.Id();
            sigma.Shuffle();
        }
    public:
        Shuffled() : T() {}
        Shuffled(size_t n, value_type t) : T(n, t), sigma{std::vector<uint>(n,0), {}} { InitPermutation(); }
        Shuffled(const Shuffled&) = default;
        Shuffled(Shuffled&&) = default;
        Shuffled& operator=(const Shuffled&) = default;
        Shuffled& operator=(Shuffled&&) = default;
        Shuffled(std::initializer_list<value_type> _) : T(_), sigma(T::size(), 0) { InitPermutation(); }

        auto& operator[](int i) noexcept { return this->T::operator[](sigma[i]); }
        const auto& operator[](int i) const noexcept { return this->T::operator[](sigma[i]); }
        void Shuffle() { sigma.Shuffle(); }
    };

    template <class T>
    void Shuffle(T& _) {
        if constexpr (std::is_base_of_v<IsShufflable, T>) {
            _.Shuffle();
        }
    }
};


namespace shooters {

// no indents because the code has a lot of conditionals which make it hard to read already



template <class Grid>
struct Simple {
    ShootingGrid<Grid>* field = nullptr;

    std::random_device random_device;
    std::mt19937 randgen;

    permutations::Permutation permut;

    bool should_fire_to_next = false;
    Ship::Orientation which_direction_next;


    Simple(int shapex, int shapey) :  field(nullptr), randgen(random_device()), permut{std::vector<uint>(shapex * shapey, 0), {}} {
        permut.Id();
    };

    void set(ShootingGrid<Grid>& f) { field = &f; }

    void shoot() {
        permut.Shuffle();
        // go over the fields in a random order
        for (auto i : permut) {
            if (field->is_all_ships_sunk()) {
                return;
            }
            pos p;
            p.x = i % field->shapex();
            p.y = i / field->shapex();
            // Check if there could be a ship in the cell, if not, skip
            auto can_be_ship = [&](pos p) -> bool {
                for (auto& neigh : Field::neigh_shifts) {
                    if (!field->is_allowed(p + neigh)) {
                        return false;
                    }
                    if (field->was_sunk(p + neigh)) {
                        return false;
                    }
                }
                return true;
            };
            if (field->was_hit(p)) {
                continue;
            }
            if (!can_be_ship(p)) {
                continue;
            }

            auto res = field->attack(p);
            
            // If we hit a ship, then we look at its neighbours to hit its other parts
            if (res.status == Ship::HitStatus::Damaged) {
                for (auto o : Ship::orientation_list) {
                    pos p_here = p;
                    pos incr = Ship::get_increment(o);
                    p_here = p_here + incr;
                    Ship::AttackResult res_in_dir;
                    do {
                        if (!can_be_ship(p)){
                            break;
                        }
                        res_in_dir = field->attack(p_here);
                        p_here = p_here + p;
                    } while (res_in_dir.status == Ship::HitStatus::Damaged);
                    if (res_in_dir.status == Ship::HitStatus::Sunk) {
                        break;
                    }
                }
            }
        }
    }
};
};