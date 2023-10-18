# Sea Battle Battleground

This is a simple implementation of the Sea Battle game.

The point is to write a good heuristic algorithm which solves it.

Sample algorithms are given in `simple_impl.hpp`:
 - namespace `placement_generators` contains a simple somewhat-random ship placement generator
 - namespace `shooters` contains a simple algorithm which shoots at ships, using human-like heuristics

To write your own algorithm, copy-n-change the given reference algorithms

The code is written in template to assure correctness and make it impossible for the user to change the game rules

Run with CMake to get the estimates for the current models used in main.cpp