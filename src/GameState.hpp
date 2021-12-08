#pragma once
#include <iostream>
#include <string>
#include <vector>

#define stringify(s) #s

// log non string
#define LOGN(s) cout << stringify(s) << endl;

// log string
#define LOGS(s) cout << std::string(s) << endl;

using namespace std;

enum Player {
  Me,
  Ai  // Artifical itilligence
};

enum Direction { Right, Left, Auto };

class GameState;

struct Step {
  int hole;
  Direction dir;
};

class GameState {
 private:
  vector<int> ai_holes;
  vector<int> my_holes;
  int ai_win_hole;
  int my_win_hole;
  Player next_player;

  vector<bool> ai_was_changed;
  vector<bool> my_was_changed;

  int forbidden_hole;

 public:
  int getForbiddenHole() { return forbidden_hole; }

  int heuristic() { return my_win_hole - ai_win_hole; }

  void setNext(Player p) { next_player = p; }

  vector<Step> availableSteps() {
    vector<Step> ret;

    for (int i = 0; i < ai_holes.size(); i++) {
      int hole;
      if (next_player == Ai) {
        hole = ai_holes[i];
      } else if (next_player == Me) {
        hole = my_holes[i];
      }

      if (hole != 0) {
        if (i < 2) {
          ret.push_back({i, Auto});
        } else if (i > 2) {
          ret.push_back({i, Auto});
        } else if (i == 2) {
          ret.push_back({i, Right});
          ret.push_back({i, Left});
        }
      }
    }

    return ret;
  }

  GameState& operator=(const GameState& other) {
    this->ai_holes = other.ai_holes;
    this->my_holes = other.my_holes;
    this->ai_win_hole = other.ai_win_hole;
    this->my_win_hole = other.my_win_hole;
    this->next_player = other.next_player;
    this->ai_was_changed = other.ai_was_changed;
    this->my_was_changed = other.my_was_changed;
    this->forbidden_hole = other.forbidden_hole;
    return *this;
  }

  int countNonZeroFor(Player p) {
    int c = 0;
    if (p == Me) {
      for (auto hole : my_holes) {
        if (hole != 0) {
          c++;
        }
      }
    } else if (p == Ai) {
      for (auto hole : ai_holes) {
        if (hole != 0) {
          c++;
        }
      }
    }
    return c;
  }

 private:
  void checkForAvailabilityHole(size_t hole_num, Player p) {
    if (p == Me) {
      // check ai
      if (my_holes[hole_num] == 0 || my_holes[hole_num] > 50) {
        throw runtime_error("not available");
      }
    } else if (p == Ai) {
      // check my
      if (ai_holes[hole_num] == 0 || ai_holes[hole_num] > 50) {
        throw runtime_error("not available");
      }
    }
  }

  void moveAs(size_t hole_num, Player player, Direction dir = Direction::Auto) {
    try {
      checkForAvailabilityHole(hole_num, player);
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      cout << "hole: " << hole_num << endl
           << "balls: "
           << ((player == Me) ? my_holes[hole_num] : ai_holes[hole_num]) << endl
           << "dir: " << dir << endl;
    }

    // here do moves and check holes if can be moved to win hole
    Direction local_dir;
    if (hole_num == 2) {
      local_dir = dir;
    } else if (hole_num > 2 && hole_num <= 4) {
      local_dir = Direction::Right;
    } else if (hole_num < 2 && hole_num >= 0) {
      local_dir = Direction::Left;
    } else {
      throw runtime_error("moveAs: no such hole number");
    }

    if (hole_num == 2 && dir == Auto) {
      throw runtime_error("moveAs: center hole cant have 'Auto' direction");
    }

    // check for forbidden hole was outside of function, so
    // before making the move whe should set forbidden hole to -1
    forbidden_hole = -1;

    int c = takeBallsFrom(hole_num, player);

    int opposite_c = 0;
    if (player == Me) {
      opposite_c = ai_holes[hole_num];
    } else if (player == Ai) {
      opposite_c = my_holes[hole_num];
    }

    const int src_c = c;

    bool for_me = (player == Player::Me);
    int i = hole_num;
    updI(i, for_me, local_dir);

    makeIndexCorrect(i, for_me, local_dir);

    for (; c > 0; c--) {
      getElemByIndexAndPlayer(i, for_me) += 1;
      if (player == Player::Me && !for_me) {
        ai_was_changed[i] = 1;
      } else if (player == Player::Ai && for_me) {
        my_was_changed[i] = 1;
      }

      updI(i, for_me, local_dir);
      makeIndexCorrect(i, for_me, local_dir);
    }

    checkForTwoOrFourToUpdateScore(player);

    // TODO: add checking for c==1 and side hole(0 or 4) to make hole forbidden
    // for another move
    if (src_c == 1 && (hole_num == 0 || hole_num == 4) && opposite_c == 0) {
      forbidden_hole = hole_num;
    }
  }

  void checkForTwoOrFourToUpdateScore(Player p) {
    if (p == Me) {
      for (int i = 0; i < ai_holes.size(); i++) {
        if (ai_was_changed[i]) {
          if (ai_holes[i] == 2 || ai_holes[i] == 4) {
            my_win_hole += ai_holes[i];
            ai_holes[i] = 0;
          }
        }
      }
    } else if (p == Ai) {
      for (int i = 0; i < my_holes.size(); i++) {
        if (my_was_changed[i]) {
          if (my_holes[i] == 2 || my_holes[i] == 4) {
            ai_win_hole += my_holes[i];
            my_holes[i] = 0;
          }
        }
      }
    }

    ai_was_changed = vector<bool>(5, 0);
    my_was_changed = vector<bool>(5, 0);
  }

  int& getElemByIndexAndPlayer(int i, bool for_me) {
    if (for_me)
      return my_holes[i];
    else if (!for_me)
      return ai_holes[i];
  }

  int takeBallsFrom(size_t hole_num, Player p) {
    if (p == Player::Me) {
      auto ret = my_holes[hole_num];
      my_holes[hole_num] = 0;
      return ret;
    } else if (p == Player::Ai) {
      auto ret = ai_holes[hole_num];
      ai_holes[hole_num] = 0;
      return ret;
    } else {
      throw runtime_error("takeBallsFrom: no such type of player " +
                          to_string(p));
    }
  }

  void makeIndexCorrect(int& i, bool& for_me, Direction& d) {
    if (i < 0) {
      i = 0;
      for_me = !for_me;
      d = (d == Right) ? Left : Right;
    } else if (i > 4) {
      i = 4;
      for_me = !for_me;
      d = (d == Right) ? Left : Right;
    }
  }

  void updI(int& i, bool for_me, Direction d) {
    if (d == Right && for_me)
      i++;
    else if (d == Left && for_me)
      i--;
    else if (d == Right && !for_me)
      i++;
    else if (d == Left && !for_me)
      i--;
  }

 public:
  GameState() {
    srand(time(0));
    ai_holes = vector<int>(5, 5);
    my_holes = vector<int>(5, 5);
    ai_win_hole = 0;
    my_win_hole = 0;

    next_player = Me;

    ai_was_changed = vector<bool>(5, 0);
    my_was_changed = vector<bool>(5, 0);

    forbidden_hole = -1;
  }

  GameState(vector<int> ai_holes, vector<int> my_holes) : GameState() {
    this->ai_holes = ai_holes;
    this->my_holes = my_holes;
  }

  void moveAsPlayer(size_t hole_num, Direction direction = Auto) {
    moveAs(hole_num, Player::Me, direction);
  }

  void moveAsComputer(size_t hole_num, Direction direction = Auto) {
    moveAs(hole_num, Player::Ai, direction);
  }

  void moveNext(size_t hole_num, Direction direction = Auto) {
    if (next_player == Me) {
      moveAsPlayer(hole_num, direction);
      next_player = Ai;
    } else if (next_player == Ai) {
      moveAsComputer(hole_num, direction);
      next_player = Me;
    } else
      throw runtime_error("moveNext: no such player type");
  }

  void printState() {
    cout << "ai holes: ";
    for (auto elem : ai_holes) cout << elem << " ";
    cout << endl;

    cout << "my holes: ";
    for (auto elem : my_holes) cout << elem << " ";
    cout << endl;

    cout << "----score----" << endl;
    cout << "my:" << my_win_hole << " ai:" << ai_win_hole << endl;

    cout << endl << endl;
  }

  vector<int> getAiHoles() { return ai_holes; }

  vector<int> getMyHoles() { return my_holes; }

  int getAiScore() { return ai_win_hole; }

  int getMyScore() { return my_win_hole; }
};
