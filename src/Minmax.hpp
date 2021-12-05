#pragma once
#include "GameState.hpp"

#include <map>

using namespace std;

typedef GameState node_t;

const int min_inf = -51;
const int max_inf = 51;

struct alphabeta_ret
{
    Step step;
    int heuristic;

    alphabeta_ret& operator=(const alphabeta_ret& other) {
        this->step = other.step;
        this->heuristic = other.heuristic;
    }
};

vector<GameState> do_steps(GameState src, vector<Step> steps) {
    vector<GameState> ret;
    for(auto step : steps) {
        GameState temp = src;
        temp.moveNext(step.hole, step.dir);
        ret.push_back(temp);
    }
    return ret;
}

alphabeta_ret max(alphabeta_ret a, alphabeta_ret b) {
    if(a.heuristic > b.heuristic) {
        return a;
    } else if(a.heuristic < b.heuristic) {
        return b;
    } else {
        return a;
    }
}

alphabeta_ret min(alphabeta_ret a, alphabeta_ret b) {
    if(a.heuristic < b.heuristic) {
        return a;
    } else if(a.heuristic > b.heuristic) {
        return b;
    } else {
        return a;
    }
}

alphabeta_ret alphabeta(node_t node, Step done_step, int depth, int a, int b, bool max_player) {
    if(depth == 0) {
        return {done_step, node.heuristic()};
    }

    if(max_player) {
        alphabeta_ret value = {{}, min_inf};
        auto steps = node.availableSteps();
        auto childs = do_steps(node, steps);
        for(int i = 0; i < steps.size(); i++) {
            alphabeta_ret res = alphabeta(childs[i], steps[i], depth-1, a, b, false);
            value = max(value, res);
            int a = max(a, value.heuristic);
            if(a >= b) {
                continue;
            }
        }
        return value;
    } else {
        alphabeta_ret value = {{}, max_inf};
        auto steps = node.availableSteps();
        auto childs = do_steps(node, steps);
        for(int i = 0; i < steps.size(); i++) {
            alphabeta_ret res = alphabeta(childs[i], steps[i], depth-1, a, b, true);
            value = min(value, res);
            int b = min(b, value.heuristic);
            if(b <= a) {
                continue;
            }
        }
        return value;
    }
}

Step aiDecision(GameState state, string level = "easy") {
    int level_int = (level == "easy") ? 2 : (level == "medium") ? 4 : (level == "hard") ? 8 : 2;
    return alphabeta(state, {}, level_int, min_inf, max_inf, true).step;
}