#include <iostream>

#include "GameState.hpp"

using namespace std;

int main() {
  GameState gs;
  gs.printState();

  for (int i = 0; i < 1000; i++) {
    int hole_num = rand() % 5;
    if (hole_num == 2) {
      gs.moveNext(hole_num, Direction(rand() % 2));
    } else {
      gs.moveNext(hole_num, Auto);
    }

    gs.printState();
  }
}