#include "sea_battle.hpp"
#include "simple_impl.hpp"
#include <iostream>


int main() {
    // This describes our playing field
    // Container: std::vector, size 10x10
    //
    //                4 submarines 1x1
    //                |  3 destroyers 1x2
    //                |  |  2 battleships 1x3
    //                |  |  |  1 carrier 1x4
    Grid<std::vector, 4, 3, 2, 1> grid(10,10,0);
    // final result
    long long sum = 0;
    // number of iterations to measure
    unsigned iters = 100000;

    auto placement = PlacementGrid(grid);
    auto shooting = ShootingGrid(grid);

    placement_generators::Simple<decltype(placement)> pl;
    shooters::Simple<decltype(grid)> sh(10,10);
    
    for (unsigned i = 0; i < iters; i++) {
        pl.set(placement);
        // generate the ship configuration
        pl.generate();
        // std::cout << grid.ascii_repr() << '\n';

        // if its malformed, abort
        if (!placement.is_full()) {
            throw 'c';
        }

        sh.set(shooting);
        // Play the game until all the ships were sunk
        sh.shoot();
        // std::cout << grid.ascii_repr() << '\n';

        // If not all ships were sunk, abort
        if (grid.ship_count != 0) {
            throw 1;
        }
        sum = shooting.get_shot_count();
        // if (i == iters - 1) {
        //     std::cout << grid.ascii_repr();
        // }
        
        // prepare for new iteration
        grid.clear();
   }
   // The fimal result;
    std::cout << ((double)sum) / (double)iters << '\n';
}