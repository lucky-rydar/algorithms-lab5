#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "GameState.hpp"
#include "Minmax.hpp"

using namespace std;
using namespace sf;

string font_path = "/usr/share/fonts/TTF/pixel_style_font.ttf";

class Front {
 private:
  GameState gs;
  vector<sf::RectangleShape> ai_holes;
  vector<sf::Text> ai_nums;

  sf::RectangleShape left_rec;
  Text left_score;

  vector<sf::RectangleShape> my_holes;
  vector<sf::Text> my_nums;

  sf::RectangleShape right_rec;
  Text right_score;

  sf::RectangleShape bottom_panel;
  sf::Text heuristic_points;
  sf::Text game_level;

  Font font;

  bool game_end = false;
  string level;

 public:
  Front(int argc, char** argv, int x = 150, float border = -2,
        float font_size = 50) {
    // gs = GameState({0, 0, 1, 18, 0},
    //                {1, 0, 0, 0, 0});

    font.loadFromFile(font_path);

    ai_holes = vector<sf::RectangleShape>(5, RectangleShape({x, x}));
    for (int i = 0; i < ai_holes.size(); i++) {
      auto& hole = ai_holes[i];
      hole.setPosition({x * i + x, 0});
      hole.setFillColor(Color(253, 188, 94));
      hole.setOutlineThickness(border);
      hole.setOutlineColor(Color::Black);
    }

    my_holes = vector<sf::RectangleShape>(5, RectangleShape({x, x}));
    for (int i = 0; i < my_holes.size(); i++) {
      auto& hole = my_holes[i];
      hole.setPosition({x * i + x, x});
      hole.setFillColor(Color(253, 188, 94));
      hole.setOutlineThickness(border);
      hole.setOutlineColor(Color::Black);
    }

    left_rec = RectangleShape({x, 2 * x});
    left_rec.setPosition({0, 0});
    left_rec.setFillColor(Color(253, 188, 94));
    left_rec.setOutlineThickness(border);
    left_rec.setOutlineColor(Color::Black);

    left_score.setFont(font);
    left_score.setColor(Color::Red);
    left_score.setCharacterSize(font_size);

    right_score.setFont(font);
    right_score.setColor(Color::Blue);
    right_score.setCharacterSize(font_size);

    ai_nums = vector<Text>(5, Text());
    for (auto& ai_num : ai_nums) {
      ai_num.setFont(font);
      ai_num.setColor(Color::Red);
      ai_num.setCharacterSize(font_size);
    }

    my_nums = vector<Text>(5, Text());
    for (auto& my_num : my_nums) {
      my_num.setFont(font);
      my_num.setColor(Color::Blue);
      my_num.setCharacterSize(font_size);
    }

    right_rec = RectangleShape({x, 2 * x});
    right_rec.setPosition({6 * x, 0});
    right_rec.setFillColor(Color(253, 188, 94));
    right_rec.setOutlineThickness(border);
    right_rec.setOutlineColor(Color::Black);

    bottom_panel = RectangleShape({x * 7, x});
    bottom_panel.setPosition({0, x * 2});
    bottom_panel.setFillColor(Color(253, 188, 94));
    bottom_panel.setOutlineThickness(border);
    bottom_panel.setOutlineColor(Color::Black);

    heuristic_points.setFont(font);
    heuristic_points.setColor(Color::Black);
    heuristic_points.setCharacterSize(font_size * 0.8);

    game_level.setFont(font);
    game_level.setColor(Color::Black);
    game_level.setCharacterSize(font_size * 0.8);

    if (argc != 2) throw runtime_error("not enough arguments");
    level = string(argv[1]);

    if (level != "easy" && level != "medium" && level != "hard")
      throw runtime_error(
          "level argument not correct (should be 'easy', 'medium' of 'hard')");
  }

  int exec() {
    sf::RenderWindow window(sf::VideoMode(1050, 450), "Alemungula");
    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
        else if (event.type == sf::Event::MouseButtonReleased) {
          if (event.mouseButton.button == Mouse::Left) {
            Mouse m;
            mouseLeftButtonReleased(
                {m.getPosition(window).x, m.getPosition(window).y});
          }
        }
      }

      // do updates here
      update();

      window.clear();

      for (auto hole : ai_holes) window.draw(hole);
      for (auto hole : my_holes) window.draw(hole);
      window.draw(right_rec);
      window.draw(left_rec);
      window.draw(bottom_panel);

      window.draw(left_score);
      window.draw(right_score);

      for (auto ai_num : ai_nums) {
        window.draw(ai_num);
      }

      for (auto my_num : my_nums) {
        window.draw(my_num);
      }

      window.draw(heuristic_points);
      window.draw(game_level);

      window.display();
    }
    return 0;
  }

 private:
  void update() { updateText(); }

  void mouseLeftButtonReleased(Vector2f pos) {
    if (game_end) {
      cout << "game over" << endl;
      return;
    }

    bool ai_turned = false;

    for (int i = 0; i < my_holes.size(); i++) {
      if (gs.getForbiddenHole() == i && gs.countNonZeroFor(Me) == 1) {
        game_end = true;
      }

      if (my_holes[i].getGlobalBounds().contains(pos) &&
          gs.getMyHoles()[i] != 0 && gs.getForbiddenHole() != i) {
        Direction dir_choose;
        if (i == 2) {
          float center_x = (float)my_holes[i].getGlobalBounds().left +
                           my_holes[i].getGlobalBounds().width / 2.0f;
          if (pos.x > center_x) {
            dir_choose = Direction::Right;
          } else {
            dir_choose = Direction::Left;
          }
          gs.moveAsPlayer(i, dir_choose);
        } else {
          gs.moveAsPlayer(i, Direction::Auto);
        }
        cout << "Me: hole: " << (i) << " - "
             << ((dir_choose == Right)  ? "Right"
                 : (dir_choose == Left) ? "Left"
                                        : "Auto")
             << endl;

        // then it is turn of ai
        aiTurn();
        ai_turned = true;
      }
    }

    if (allMyZero()) {
      game_end = true;
    }

    checkForEndState();
  }

  bool allZero(vector<int> holes) {
    for (auto hole : holes) {
      if (hole != 0) return false;
    }
    return true;
  }

  bool allAiZero() { return allZero(gs.getAiHoles()); }

  bool allMyZero() { return allZero(gs.getMyHoles()); }

  void aiTurn() {
    if (allAiZero()) {
      game_end = true;
    }

    if (game_end) {
      return;
    }

    /*int hole;
    do {
        hole = rand() % 5;
    } while(gs.getAiHoles()[hole] == 0);

    gs.moveAsComputer(hole, hole == 2 ? Direction(rand() % 2) :
    Direction::Auto); cout << "exec ai turn, choosen hole=" << hole << endl;*/

    auto step = aiDecision(gs, level);
    cout << "Ai: hole: " << (step.hole) << " - "
         << ((step.dir == Right)  ? "Right"
             : (step.dir == Left) ? "Left"
                                  : "Auto")
         << endl;
    cout << endl;
    gs.moveAsComputer(step.hole, step.dir);
  }

  bool checkForEndState() {
    int c = 0;
    for (auto hole : gs.getMyHoles()) c += hole;
    for (auto hole : gs.getAiHoles()) c += hole;
    if (c <= 2) game_end = true;
  }

  void updateText() {
    left_score.setString(to_string(gs.getAiScore()));
    auto left_text_bounds = left_score.getLocalBounds();
    left_score.setOrigin(left_text_bounds.left + left_text_bounds.width / 2.0f,
                         left_text_bounds.top + left_text_bounds.height / 2.0f);
    left_score.setPosition(left_rec.getPosition().x + left_rec.getSize().x / 2,
                           left_rec.getPosition().y + left_rec.getSize().y / 2);

    right_score.setString(to_string(gs.getMyScore()));
    auto right_text_bounds = right_score.getLocalBounds();
    right_score.setOrigin(
        right_text_bounds.left + right_text_bounds.width / 2.0f,
        right_text_bounds.top + right_text_bounds.height / 2.0f);
    right_score.setPosition(
        right_rec.getPosition().x + right_rec.getSize().x / 2,
        right_rec.getPosition().y + right_rec.getSize().y / 2);

    for (int i = 0; i < ai_nums.size(); i++) {
      auto rect = ai_holes[i];

      ai_nums[i].setString(to_string(gs.getAiHoles()[i]));
      auto bounds = ai_nums[i].getLocalBounds();
      ai_nums[i].setOrigin(bounds.left + bounds.width / 2.0f,
                           bounds.top + bounds.height / 2.0f);
      ai_nums[i].setPosition(rect.getPosition().x + rect.getSize().x / 2,
                             rect.getPosition().y + rect.getSize().y / 2);
    }

    for (int i = 0; i < my_nums.size(); i++) {
      auto rect = my_holes[i];

      my_nums[i].setString(to_string(gs.getMyHoles()[i]));
      auto bounds = my_nums[i].getLocalBounds();
      my_nums[i].setOrigin(bounds.left + bounds.width / 2.0f,
                           bounds.top + bounds.height / 2.0f);
      my_nums[i].setPosition(rect.getPosition().x + rect.getSize().x / 2,
                             rect.getPosition().y + rect.getSize().y / 2);
    }

    // heuristic_points
    heuristic_points.setString("Heuristic " + to_string(gs.heuristic()));
    auto heuristic_bounds = heuristic_points.getLocalBounds();
    heuristic_points.setOrigin(
        0, heuristic_bounds.top + heuristic_bounds.height / 2.0f);
    heuristic_points.setPosition(
        20, bottom_panel.getPosition().y + bottom_panel.getSize().y / 6);

    // game_level
    game_level.setString("Game level " + level);
    auto game_level_bounds = game_level.getLocalBounds();
    game_level.setOrigin(
        0, game_level_bounds.top + game_level_bounds.height / 2.0f);
    game_level.setPosition(20, bottom_panel.getPosition().y +
                                   bottom_panel.getSize().y / 6 +
                                   game_level.getLocalBounds().height);
  }
};
