//
// Created by Anton Olsson on 2020-02-16.
//

#pragma once

#include "gameObject.h"
#include "avancezlib.h"
#include "component.h"
#include "game.h"
#include <string>

class GUI : public GameObject {

    unsigned int gameWidth = 0;
    unsigned int gameHeight = 0;
    unsigned int score = 0;

public:

    unsigned int getGameWidth() const {
        return gameWidth;
    }

    unsigned int getGameHeight() const {
        return gameHeight;
    }

    unsigned int getScore() const {
        return score;
    }

    GUI(unsigned int _gameWidth, unsigned int _gameHeight) {
        gameWidth = _gameWidth;
        gameHeight = _gameHeight;
    }

    void update(unsigned int _score) {
        score = _score;
        for (auto& component : components) {
            component->update(0);
        }
    }
};

class GUIRenderComponent : public RenderComponent {

    const SDL_Color BLACK {0, 0, 0, 255};
    const SDL_Color YELLOW {255, 255, 0, 255};
    const SDL_Color WHITE {255, 255, 255, 255};

    const SpriteSheet BARS_SHEET {"resource/sprites/bars.png", {192, 192, 192,
                                                                255}};
    const Vector2D BAR_SCREEN_POS {16, 24};
    const int BAR_PIECE_MAX_HP = 4;
    const SpriteProperties WHOLE_BAR {0, {0, 8}, {8, 56}};
    const SpriteProperties THREE_HP_BAR {0, {16, 24}, {8, 8}};
    const SpriteProperties TWO_HP_BAR {0, {16, 40}, {8, 8}};
    const SpriteProperties ONE_HP_BAR {0, {16, 56}, {8, 8}};
    const SpriteProperties ZERO_HP_BAR {0, {144, 56}, {8, 8}};
    std::map<int, SpriteProperties> hpToSpriteMap {{1, THREE_HP_BAR}, {2, TWO_HP_BAR}, {3, ONE_HP_BAR},
                                                   {4, ZERO_HP_BAR}};

    const unsigned int GUI_HEIGHT = 15;
    const char *const FONT_FILE = "resource/PixelNES.otf";
    const int FONT_SIZE = 10;
    const int SCORE_NUM_DIGITS = 7;
    const std::string GAME_OVER_MESS = "GAME OVER";
    const std::string LEVEL_WON_MESS = "LEVEL WON";

    unsigned int spriteYPos = (int) (GUI_HEIGHT * 1.6);
    unsigned int scoreXPos = 0;
    unsigned int messageYPos = 0;

    std::string score = "0000000";

    GUI* gui{};

    AvancezLib* engine;

public:

    void create(GUI *_gui, AvancezLib *_engine) {
        gui = _gui;
        engine = _engine;
        //sprite = engine->createSprite(BARS_SHEET);
        engine->setFont(FONT_FILE, FONT_SIZE, BLACK);
        scoreXPos = gui->getGameWidth() / 2;
        messageYPos = gui->getGameHeight() / 2;
    }

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "bugprone-narrowing-conversions"

//#pragma clang diagnostic pop

    // As in the original game!
    void replace0ByO() {
        for (char & i : score) {
            if (i == '0') i = 'O';
        }
    }

    // Draw text with shadow
    void drawText(const std::string &msg, AvancezLib::TextAlign textAlign, unsigned int yPos, const SDL_Color color) {
        replace0ByO();
        engine->drawText((int) scoreXPos, (int) (yPos + 1), msg.c_str(), textAlign,
                         BLACK);
        engine->drawText((int) (scoreXPos + 1), (int) yPos, msg.c_str(), textAlign,
                         BLACK);
        engine->drawText((int) scoreXPos, (int) yPos, msg.c_str(), textAlign,
                         color);
    }

    std::string getScore() {
        score = std::to_string(gui->getScore());
        while (score.size() < SCORE_NUM_DIGITS) {
            score.insert(0, "0"); // Pad with zeros if necessary
        }
        return score;
    }

    void update(float dt) override {
        score = getScore();
        drawText(score, engine->CENTER, spriteYPos, WHITE);
        if (gui->isGameOver()) drawText(GAME_OVER_MESS, engine->CENTER, messageYPos, YELLOW);
        else if (gui->isLevelWon()) drawText(LEVEL_WON_MESS, engine->CENTER, messageYPos, YELLOW);
    }

};
