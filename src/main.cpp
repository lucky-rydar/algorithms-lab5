#include <iostream>

#include "GameState.hpp"
#include "Front.hpp"

using namespace std;

int main(int argc, char** argv) {
  Front f(argc, argv);
  return f.exec();
}