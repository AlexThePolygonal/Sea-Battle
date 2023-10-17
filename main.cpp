#include "sea_battle.hpp"
#include "simple_impl.hpp"
#include <iostream>

int main() {
    Grid<std::vector, 0, 1, 1, 2> grid(8,8,0);
    auto placement = PlacementGrid(grid);
    auto shooting = ShootingGrid(grid);
    PlacementGenerator<decltype(placement)> pl;
    pl.set(placement);
    pl.generate();
    std::cout << std::boolalpha;
    std::cout << placement.is_full() << '\n';
    Shooter<decltype(grid)> sh(8,8);
    sh.set(shooting);
    sh.shoot();
    std::cout << grid.ship_count << '\n';
    std::cout << shooting.get_count() << '\n';
    std::cout << grid.ascii_print() << '\n';
}