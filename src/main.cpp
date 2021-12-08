#include <iostream>

#include "Front.hpp"
#include "GameState.hpp"

using namespace std;

int main(int argc, char** argv) {
  Front f(argc, argv);
  return f.exec();
}