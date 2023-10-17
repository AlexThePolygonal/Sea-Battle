#include "sea_battle.hpp"
#include <iostream>
#include <algorithm>

template <class Placement>
struct PlacementGenerator {
    Placement* field = nullptr;
    std::random_device random_device;
    std::mt19937 randgen;


    PlacementGenerator() : field(nullptr), randgen(random_device()){};
    PlacementGenerator(Placement& f) : field(&f), randgen(random_device()) {}

    void set(Placement& f) { field = &f; }

    void generate() {
        restart:
        auto max_ship_counts = field->max_ship_counts();
        int iters = 0;
        std::uniform_int_distribution<int> rand_pos_x(0, field->shapex() -1);
        std::uniform_int_distribution<int> rand_pos_y(0, field->shapey() -1);
        std::uniform_int_distribution<int> rand_orient(0, 3);
        for (int i = 3; i >= 0; i--) {
            for (int c = 0; c < max_ship_counts[i];) {
                iters++;
                pos p;
                p.x = rand_pos_x(randgen);
                p.y = rand_pos_y(randgen);
                // auto o = Ship::orientations[rand_orient(randgen)];
                auto o = Ship::orientations[0];
                auto t = Ship::ship_types[i];
                if (field->place_ship(p, t, o) == 0) {
                    if (iters >= 1000000) {
                        std::cout << "Failure";
                        return;
                    }
                    if (iters >= 1000) {
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


std::random_device random_device;
std::mt19937 randgen(random_device());

struct IsShuffled {};

struct Permutation : std::vector<uint>, IsShuffled {
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
struct Shuffled : T, IsShuffled {
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
    if constexpr (std::is_base_of_v<IsShuffled, T>) {
        _.Shuffle();
    }
}

template <class Grid>
struct Shooter {
    ShootingGrid<Grid>* field = nullptr;
    std::random_device random_device;
    std::mt19937 randgen;
    Permutation permut;
    bool should_fire_to_next = false;
    Ship::Orientation which_direction_next;


    Shooter(int shapex, int shapey) :  field(nullptr), randgen(random_device()), permut{std::vector<uint>(shapex * shapey, 0), {}} {
        permut.Id();
    };

    void set(ShootingGrid<Grid>& f) { field = &f; }

    void shoot() {
        permut.Shuffle();
        for (auto i : permut) {
            if (field->all_shot()) {
                return;
            }
            pos p;
            p.x = i % field->shapex();
            p.y = i / field->shapex();
            if (field->was_hit(p)) {
                continue;
            }
            bool cond = false;
            for (auto& neigh : Field::neigh_shifts) {
                if (field->is_allowed(p+neigh) && field->was_sunk(p+neigh) ) {
                    cond = true;
                    break;
                }
            }
            if (cond) {
                continue;
            }
            auto res = field->attack(p);
            if (res.status == Ship::HitStatus::Damaged) {
                for (auto o : Ship::orientations) {
                    pos p_here = p;
                    pos incr = Ship::get_increment(o);
                    p_here = p_here + incr;
                    Ship::AttackResult res_in_dir;
                    do {
                        if (field->was_hit(p_here)) {
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