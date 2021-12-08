#pragma once
#include <map>

#include "GameState.hpp"

using namespace std;

typedef GameState node_t;

const int min_inf = -51;
const int max_inf = 51;

struct alphabeta_ret {
  Step step;
  int heuristic;

  alphabeta_ret& operator=(const alphabeta_ret& other) {
    this->step = other.step;
    this->heuristic = other.heuristic;
  }
};

vector<GameState> do_steps(GameState src, vector<Step> steps) {
  vector<GameState> ret;
  for (auto step : steps) {
    GameState temp;
    temp = src;
    temp.moveNext(step.hole, step.dir);

    // DELETE ME!
    // for(auto item: temp.getAiHoles()) cout << item << " ";
    // cout << endl;
    // for(auto item: temp.getMyHoles()) cout << item << " ";
    // cout << endl << endl;

    ret.push_back(temp);
  }
  return ret;
}

alphabeta_ret max(alphabeta_ret a, alphabeta_ret b) {
  if (a.heuristic > b.heuristic) {
    return a;
  } else if (a.heuristic < b.heuristic) {
    return b;
  } else {
    return a;
  }
}

alphabeta_ret min(alphabeta_ret a, alphabeta_ret b) {
  if (a.heuristic < b.heuristic) {
    return a;
  } else if (a.heuristic > b.heuristic) {
    return b;
  } else {
    return a;
  }
}

alphabeta_ret alphabeta(node_t node, Step done_step, int depth, int a, int b,
                        bool max_player, bool first_call = true) {
  if (depth == 0) {
    return {done_step, node.heuristic()};
  }

  if (max_player)
    node.setNext(Ai);
  else
    node.setNext(Me);

  if (max_player) {
    alphabeta_ret value = {done_step, min_inf};
    auto steps = node.availableSteps();
    auto childs = do_steps(node, steps);
    for (int i = 0; i < steps.size(); i++) {
      alphabeta_ret res;
      if (first_call) {
        res = alphabeta(childs[i], steps[i], depth - 1, a, b, false, false);
      } else {
        res = alphabeta(childs[i], done_step, depth - 1, a, b, false, false);
      }
      value = max(value, res);
      int a = max(a, value.heuristic);
      if (a >= b) {
        continue;
      }
    }
    return value;
  } else {
    alphabeta_ret value = {done_step, max_inf};
    auto steps = node.availableSteps();
    auto childs = do_steps(node, steps);
    for (int i = 0; i < steps.size(); i++) {
      // alphabeta_ret res = alphabeta(childs[i], steps[i], depth-1, a, b, true,
      // false);
      alphabeta_ret res;
      if (first_call) {
        res = alphabeta(childs[i], steps[i], depth - 1, a, b, true, false);
      } else {
        res = alphabeta(childs[i], done_step, depth - 1, a, b, true, false);
      }

      value = min(value, res);
      int b = min(b, value.heuristic);
      if (b <= a) {
        continue;
      }
    }
    return value;
  }
}

Step aiDecision(GameState state, string level = "easy") {
  int level_int = (level == "easy")     ? 2
                  : (level == "medium") ? 4
                  : (level == "hard")   ? 8
                                        : 2;
  return alphabeta(state, {}, level_int, min_inf, max_inf, true).step;
}