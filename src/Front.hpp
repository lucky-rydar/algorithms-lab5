#pragma once
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "GameState.hpp"

using namespace std;
using namespace sf;

string font_path = "/usr/share/fonts/TTF/pixel_style_font.ttf";

class Front
{
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

    Font font;
public:
    Front(int x = 150, float border = -2, float font_size = 50) {
        font.loadFromFile(font_path);

        ai_holes = vector<sf::RectangleShape>(5, RectangleShape({x, x}));
        for(int i = 0; i < ai_holes.size(); i++) {
            auto& hole = ai_holes[i];
            hole.setPosition({x*i + x, 0});
            hole.setFillColor(Color(253, 188, 94));
            hole.setOutlineThickness(border);
            hole.setOutlineColor(Color::Black);
        }

        my_holes = vector<sf::RectangleShape>(5, RectangleShape({x, x}));
        for(int i = 0; i < my_holes.size(); i++) {
            auto& hole = my_holes[i];
            hole.setPosition({x*i + x, x});
            hole.setFillColor(Color(253, 188, 94));
            hole.setOutlineThickness(border);
            hole.setOutlineColor(Color::Black);
        }

        left_rec = RectangleShape({x, 2*x});
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
        for(auto& ai_num : ai_nums) {
            ai_num.setFont(font);
            ai_num.setColor(Color::Red);
            ai_num.setCharacterSize(font_size);
        }

        my_nums = vector<Text>(5, Text());
        for(auto& my_num : my_nums) {
            my_num.setFont(font);
            my_num.setColor(Color::Blue);
            my_num.setCharacterSize(font_size);
        }

        right_rec = RectangleShape({x, 2*x});
        right_rec.setPosition({6*x, 0});
        right_rec.setFillColor(Color(253, 188, 94));
        right_rec.setOutlineThickness(border);
        right_rec.setOutlineColor(Color::Black);

        bottom_panel = RectangleShape({x*7, x});
        bottom_panel.setPosition({0, x*2});
        bottom_panel.setFillColor(Color(253, 188, 94));
        bottom_panel.setOutlineThickness(border);
        bottom_panel.setOutlineColor(Color::Black);
    }

    int exec() {
        sf::RenderWindow window(sf::VideoMode(1050, 450), "Alemungula");
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // do updates here
            update();

            window.clear();

            for(auto hole : ai_holes) window.draw(hole);
            for(auto hole : my_holes) window.draw(hole);
            window.draw(right_rec);
            window.draw(left_rec);
            window.draw(bottom_panel);

            window.draw(left_score);
            window.draw(right_score);

            for(auto ai_num : ai_nums) {
                window.draw(ai_num);
            }

            for(auto my_num : my_nums) {
                window.draw(my_num);
            }

            window.display();
        }
        return 0;
    }

    void update() {
        updateText();

        /**
         * TODO: add implementation of reacting on
         * pushing on holes only for player. Add
         * opportunity to choose direction for central
         * hole.
         */
    }

    void updateText() {
        
        left_score.setString(to_string(gs.getAiScore()));
        auto left_text_bounds = left_score.getLocalBounds();
        left_score.setOrigin(left_text_bounds.left + left_text_bounds.width/2.0f, left_text_bounds.top+left_text_bounds.height/2.0f);
        left_score.setPosition(left_rec.getPosition().x + left_rec.getSize().x / 2, left_rec.getPosition().y + left_rec.getSize().y / 2);

        right_score.setString(to_string(gs.getMyScore()));
        auto right_text_bounds = right_score.getLocalBounds();
        right_score.setOrigin(right_text_bounds.left + right_text_bounds.width/2.0f, right_text_bounds.top+right_text_bounds.height/2.0f);
        right_score.setPosition(right_rec.getPosition().x + right_rec.getSize().x / 2, right_rec.getPosition().y + right_rec.getSize().y / 2);

        for(int i = 0; i < ai_nums.size(); i++) {
            auto rect = ai_holes[i];

            ai_nums[i].setString(to_string(gs.getAiHoles()[i]));
            auto bounds = ai_nums[i].getLocalBounds();
            ai_nums[i].setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            ai_nums[i].setPosition(rect.getPosition().x + rect.getSize().x / 2, rect.getPosition().y + rect.getSize().y / 2);
        }

        for(int i = 0; i < my_nums.size(); i++) {
            auto rect = my_holes[i];

            my_nums[i].setString(to_string(gs.getMyHoles()[i]));
            auto bounds = my_nums[i].getLocalBounds();
            my_nums[i].setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            my_nums[i].setPosition(rect.getPosition().x + rect.getSize().x / 2, rect.getPosition().y + rect.getSize().y / 2);
        }
    }
};
