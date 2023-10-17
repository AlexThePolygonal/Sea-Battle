#include "sea_battle.hpp"
#include "simple_impl.hpp"
#include <iostream>

template <class T> using vector = std::vector<T>;

int main() {
    Grid<vector, 4, 3, 2, 1> grid(10,10,0);
    long long sum = 0;
    unsigned iters = 5;
    for (unsigned i = 0; i < iters; i++) {
        auto placement = PlacementGrid(grid);
        auto shooting = ShootingGrid(grid);
        PlacementGenerator<decltype(placement)> pl;
        pl.set(placement);
        pl.generate();
        Shooter<decltype(grid)> sh(10,10);
        // std::cout << grid.ascii_print() << '\n';

        sh.set(shooting);
        sh.shoot();
        std::cout << grid.ascii_print() << '\n';

        if (grid.ship_count != 0) {
            throw 1;
        }
        sum = shooting.get_shot_count();
        grid.clear();
    }
    std::cout << ((double)sum) / (double)iters << '\n';
}