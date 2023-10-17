#include "sea_battle.hpp"
#include "simple_impl.hpp"
#include <iostream>

template <class T> using vector = std::vector<T>;



/*

Compile with "g++ c++17 main.cpp"

*/

int main() {
    Grid<vector, 4, 3, 2, 1> grid(10,10,0);
    long long sum = 0;
    unsigned iters = 100000;
    auto placement = PlacementGrid(grid);
    auto shooting = ShootingGrid(grid);
    PlacementGenerator<decltype(placement)> pl;
    Shooter<decltype(grid)> sh(10,10);
    
    for (unsigned i = 0; i < iters; i++) {
        pl.set(placement);
        pl.generate();
        // std::cout << grid.ascii_print() << '\n';

        sh.set(shooting);
        sh.shoot();
        // std::cout << grid.ascii_print() << '\n';

        if (grid.ship_count != 0) {
            throw 1;
        }
        sum = shooting.get_shot_count();
        grid.clear();
    }
    std::cout << ((double)sum) / (double)iters << '\n';
}