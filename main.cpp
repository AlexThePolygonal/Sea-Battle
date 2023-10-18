#include "sea_battle.hpp"
#include "simple_impl.hpp"
#include <iostream>


int main() {
    Grid<std::vector, 4, 3, 2, 1> grid(10,10,0);
    long long sum = 0;
    unsigned iters = 100000;
    auto placement = PlacementGrid(grid);
    auto shooting = ShootingGrid(grid);
    placement_generators::Simple<decltype(placement)> pl;
    shooters::Simple<decltype(grid)> sh(10,10);
    
    for (unsigned i = 0; i < iters; i++) {
        pl.set(placement);
        pl.generate();
        // std::cout << grid.ascii_repr() << '\n';
        if (!placement.is_full()) {
            throw 'c';
        }

        sh.set(shooting);
        sh.shoot();
        // std::cout << grid.ascii_repr() << '\n';

        if (grid.ship_count != 0) {
            throw 1;
        }
        sum = shooting.get_shot_count();
        // if (i == iters - 1) {
        //     std::cout << grid.ascii_repr();
        // }
        grid.clear();
   }
    std::cout << ((double)sum) / (double)iters << '\n';
}