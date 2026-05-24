#pragma once

#include <raylib.h>
#include "Maze.hpp"
#include "Player.hpp"
#include "Ghost.hpp"
#include "Button.hpp"
#include "Queue.hpp"
#include <vector>

enum GameState {
    MENU,  //1
    TUTORIAL, //2
    PLAYING, //3
    JUMPSCARE, //4
    LOADING, //5
    CLEAR, //6
    EXIT //7
};

class Game {
    struct Star {
        Vector3 position;
        float size, twinkles;
    };
    GameState lastState;

    //sounds and music
    Music permBgSound;
    Music menuSound;
    Sound exit;
    Sound jumpscare;
    bool exitPlayed;

    //menu
    Button* playBtn;
    Button* exitBtn;
    Button* backBtn;
    Button* tutorBtn;
    Texture2D menuBg;

    std::vector<Star> stars;
    Queue<Ghost> ghosts;
    int ghostPerLevel;
    Maze* maze;
    Player player;
    Texture2D grass;
    Texture2D ghost;
    GameState state;
    int mazeSize;
    int level;
    float loadingTimer, loadingDuration;
    Model wallModel;
    Model grassModel;
    float jumpscareTimer, jumpscareDuration;
    bool isAtExit() const;
    public:
        Game(Model wall);
        void Update();
        void Draw();
        ~Game();
};