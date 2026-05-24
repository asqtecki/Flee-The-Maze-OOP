#include "../include/Game.hpp"
#include <iostream>
#include "../include/Maze.hpp"
#include <cmath>
#include <cstdlib>

bool Game::isAtExit() const {
    float cellSize = maze->getCellSize();
    Vector3 pos = player.getPosition();
    int col = (int)floor(pos.x/cellSize);
    int row = (int)floor(pos.z/cellSize);
    if (col<0) col = 0;
    if (row<0) row = 0;
    if (col>=maze->getColumn()) col = maze->getColumn();
    if (row>=maze->getRow()) row = maze->getRow();
    auto exit = maze->getExit();
    return (row==exit.first && col==exit.second);
}

Game::Game(Model wall) : wallModel(wall) {
    int w = GetScreenWidth(); 
    int h = GetScreenHeight();

    menuBg = LoadTexture("Graphics/menu_bg.png");

    InitAudioDevice();
    permBgSound = LoadMusicStream("Music/bgsound.mp3");
    SetMusicVolume(permBgSound, 0.25f);
    menuSound = LoadMusicStream("Music/menuMus.mp3");
    SetMusicVolume(menuSound, 0.3f);
    PlayMusicStream(menuSound);

    exit = LoadSound("Music/winner.mp3");
    exitPlayed = false;

    jumpscare = LoadSound("Music/jumpscare.mp3");

    mazeSize = 5;
    level = 1;
    loadingTimer = jumpscareTimer = 0.0f;
    loadingDuration = 2.5f;
    jumpscareDuration = 2.0f;
    ghostPerLevel = 1;
    state = MENU;
    lastState = MENU;

    playBtn = new Button("Graphics/play.png", {w/2.0f-200, h/2.0f-120}, 1.0f);
    tutorBtn = new Button("Graphics/tutorial.png", {w/2.0f-200, h/2.0f-10}, 1.0f);
    exitBtn = new Button("Graphics/exit.png", {w/2.0f-200, h/2.0f+100}, 1.0f);
    backBtn = new Button("Graphics/back.png", {20, 20}, 1.0f);

    grass = LoadTexture("Graphics/grass.png");
    SetTextureWrap(grass, TEXTURE_WRAP_REPEAT);
    ghost = LoadTexture("Graphics/ghost.png");

    maze = new Maze(mazeSize, mazeSize);
    maze->generateMaze();

    float totalSize = mazeSize * maze->getCellSize();
    Mesh plane = GenMeshPlane(totalSize, totalSize, 1, 1);
    grassModel = LoadModelFromMesh(plane);
    grassModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

    float cx, cz;
    cx = cz = mazeSize * maze->getCellSize()/2.0f;
    for (int i=0;i<150;i++) {
        Star s;
        s.position = {
            cx + (float)(std::rand()%200 - 100),
            (float)(15 + std::rand()%20),
            cz + (float)(std::rand()%200 - 100)
        };
        s.size = 0.1f + (std::rand()%10)/30.0f;
        s.twinkles = (std::rand()%100)/100.0f;
        stars.push_back(s);
    }

    auto entrance = maze->getEntrance();
    float cellSize = maze->getCellSize();
    player = Player({
        entrance.second * cellSize + cellSize/2.0f,
        1.0f,
        entrance.first * cellSize + cellSize/2.0f
    });
    float cs = maze->getCellSize();
    for (int i = 0; i < ghostPerLevel; i++) {
        int gr, gc;
        do {
            gr = std::rand() % mazeSize;
            gc = std::rand() % mazeSize;
        } while (abs(gr-entrance.first)<3 || abs(gc-entrance.second)<3);
        ghosts.enqueue(Ghost({gc*cs+cs/2.0f, 1.0f, gr*cs+cs/2.0f}));
    }
    EnableCursor();
}

void Game::Update() {
    UpdateMusicStream(permBgSound);
    UpdateMusicStream(menuSound);
    if (state!=lastState) {
        if (state==PLAYING) {
            DisableCursor();
            StopMusicStream(menuSound);
            PlayMusicStream(permBgSound);
        }
        else if (state==MENU) {
            EnableCursor();
            StopMusicStream(permBgSound);
            PlayMusicStream(menuSound);
        }
        else EnableCursor();
        lastState = state;
    }
    if (state==MENU) {
        Vector2 mouse = GetMousePosition();
        bool cl = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (playBtn->isClicked(mouse, cl)) state = PLAYING;
        if (exitBtn->isClicked(mouse, cl)) CloseWindow();
        if (tutorBtn->isClicked(mouse, cl)) state = TUTORIAL;
        return;
    }
    if (state==TUTORIAL) {
        Vector2 mouse = GetMousePosition();
        bool cl = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (backBtn->isClicked(mouse, cl)) state = MENU;
        return;
    }
    if (state==PLAYING) {
        if (!IsMusicStreamPlaying(permBgSound)) PlayMusicStream(permBgSound);
        player.Update(maze);
        auto playerCell = maze->worldToCell(player.getPosition());
        Vector3 pp = player.getPosition();
        for (auto& g : ghosts) {
            g.setTarget(playerCell, pp);
            g.Update(maze);
            if (g.isCaught(player.getPosition())) {
                state = JUMPSCARE;
                jumpscareTimer = 0.0f;
                PlaySound(jumpscare);
                PauseMusicStream(permBgSound);
            }
        }
        for (auto& s : stars) s.twinkles += GetFrameTime();
        if (isAtExit()) {
            if (!exitPlayed) {
                PlaySound(exit);
                exitPlayed = true;
            }
            state = LOADING;
            loadingTimer = 0.0f;
            PauseMusicStream(permBgSound);
        }
    }
    else if (state==JUMPSCARE) {
        jumpscareTimer += GetFrameTime();
        if (jumpscareTimer>=jumpscareDuration) {
            delete maze;
            maze = new Maze(mazeSize, mazeSize);
            maze->generateMaze();
            UnloadModel(grassModel);
            float totals = mazeSize * maze->getCellSize();
            Mesh plane = GenMeshPlane(totals, totals, 1, 1);
            grassModel = LoadModelFromMesh(plane);
            grassModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;
            auto entrance = maze->getEntrance();
            float cs = maze->getCellSize();
            player = Player({
                entrance.second * cs + cs/2.0f,
                1.0f,
                entrance.first * cs + cs/2.0f
            });
            ghosts.clear();
            for (int i = 0; i < ghostPerLevel; i++) {
                int gr, gc;
                do {
                    gr = std::rand() % mazeSize;
                    gc = std::rand() % mazeSize;
                } while (abs(gr-entrance.first)<3 || abs(gc-entrance.second)<3);
                ghosts.enqueue(Ghost({gc*cs+cs/2.0f, 1.0f, gr*cs+cs/2.0f}));
            }
            exitPlayed = false;
            state = PLAYING;
            PlayMusicStream(permBgSound);
        }
    }
    else if (state==LOADING) {
        if (level==3) {
                state = CLEAR;
                loadingTimer = 0.0f;
                return;
        }
        loadingTimer += GetFrameTime();
        if (loadingTimer>=loadingDuration) {
            mazeSize += 5;
            level++;
            delete maze;
            maze = new Maze(mazeSize, mazeSize);
            maze->generateMaze();
            UnloadModel(grassModel);
            float totalSize = mazeSize * maze->getCellSize();
            Mesh plane = GenMeshPlane(totalSize, totalSize, 1, 1);
            grassModel = LoadModelFromMesh(plane);
            grassModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;
            stars.clear();
            float cx, cz;
            cx = cz = mazeSize * maze->getCellSize()/2.0f;
            for (int i=0;i<150;i++) {
                Star s;
                s.position = {
                    cx + (float)(std::rand() % 200 - 100),
                    (float)(15 + std::rand() % 20),
                    cz + (float)(std::rand() % 200 - 100)
                };
                s.size = 0.1f + (std::rand() % 10) / 30.0f;
                s.twinkles = (std::rand() % 100) / 100.0f;
                stars.push_back(s);
            }
            auto entrance = maze->getEntrance();
            float cellSize = maze->getCellSize();
            player = Player({
                entrance.second * cellSize + cellSize/2.0f,
                1.0f,
                entrance.first * cellSize + cellSize/2.0f
            });
            ghosts.clear();
            ghostPerLevel += 2;  // +2 ghosts each level
            Queue<Ghost> newGs;
            auto ent = maze->getEntrance();
            float cs = maze->getCellSize();
            for (int i=0;i<ghostPerLevel;i++) {
                int r, c;
                do {
                    r = std::rand() % mazeSize;
                    c = std::rand() % mazeSize;
                } while (abs(r-ent.first)<3 || abs(c-ent.second)<3);
                newGs.enqueue(Ghost({c*cs+cs/2.0f, 1.0f, r*cs+cs/2.0f}));
            }
            ghosts += newGs;
            for (int i=0;i<ghosts.getSize();i++)
                ghosts[i].setRepathDelay(0.8f+(level*0.2f));
            Queue<Ghost> sg;
            for (int i=0; i<4; i++) sg.enqueue(Ghost({0,0,0}));
            if (ghosts > sg) {
                for (auto& g : ghosts) g.setRepathDelay(0.4f);
            }
            exitPlayed = false;
            state = PLAYING;
            PlayMusicStream(permBgSound);
        }
    }
    else if (state==CLEAR) {
        loadingTimer += GetFrameTime();
        if (loadingTimer>=3.0f) {
            mazeSize = 5;
            level = 1;
            ghostPerLevel = 1;
            delete maze;
            maze = new Maze(mazeSize, mazeSize);
            maze->generateMaze();
            UnloadModel(grassModel);
            float s = mazeSize * maze->getCellSize();
            Mesh plane = GenMeshPlane(s, s, 1, 1);
            grassModel = LoadModelFromMesh(plane);
            grassModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;
            auto ent = maze->getEntrance();
            float cs = maze->getCellSize();
            player = Player({
                ent.second * cs + cs/2.0f,
                1.0f,
                ent.first * cs + cs/2.0f
            });
            ghosts.clear();
            for (int i=0; i<ghostPerLevel;i++) {
                int r, c;
                do {
                    r = std::rand() % mazeSize;
                    c = std::rand() % mazeSize;
                } while (abs(r-ent.first)<3 || abs(c-ent.second)<3);
                ghosts.enqueue(Ghost({c*cs+cs/2.0f, 1.0f, r*cs+cs/2.0f}));
            }
            exitPlayed = false;
            loadingTimer = 0.0f;
            state = MENU;
        }
    }
}

void Game::Draw() {
    if (state==MENU) {
        BeginDrawing();
        ClearBackground(BLACK);
        int w = GetScreenWidth();
        DrawTexture(menuBg, 0, 0, WHITE);
        DrawText("FLEE THE MAZE",
            w/2 - MeasureText("FLEE THE MAZE", 50)/2,
            60, 50, (Color){180, 0, 0, 255}
        );
        playBtn->Draw();
        tutorBtn->Draw();
        exitBtn->Draw();
        EndDrawing();
        return;
    }
    if (state == TUTORIAL) {
        BeginDrawing();
        ClearBackground(BLACK);
        int w = GetScreenWidth();
        DrawText("HOW TO PLAY",
            w/2 - MeasureText("HOW TO PLAY", 50)/2,
            60, 50, {180, 0, 0, 255}
        );
        DrawText("WASD         -  Move",            w/2 - 200, 180, 26, WHITE);
        DrawText("Mouse        -  Look Around",     w/2 - 200, 220, 26, WHITE);
        DrawText("Shift        -  Sprint",          w/2 - 200, 260, 26, WHITE);
        DrawText("Objective    -  Reach the exit",  w/2 - 200, 320, 26, YELLOW);
        DrawText("Avoid        -  The ghosts!",     w/2 - 200, 360, 26, YELLOW);
        DrawText("3 Levels     -  Each bigger",     w/2 - 200, 400, 26, GRAY);
        DrawText("               and deadlier",     w/2 - 200, 435, 26, GRAY);
        backBtn->Draw();
        EndDrawing();
        return;
    }
    if (state==LOADING) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("LOADING NEXT LEVEL...", 450, 300, 20, WHITE);
        DrawText(TextFormat("LEVEL: %d", (level+1)), 550, 350, 20, GRAY);
        EndDrawing();
        return;
    }
    if (state==JUMPSCARE) {
        BeginDrawing();
        ClearBackground({180, 0, 0, 255});
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        DrawText("CAUGHT!", w/2 - MeasureText("CAUGHT!", 80)/2, h/2 - 60, 80, WHITE);
        DrawText("Restarting level...", w/2 - MeasureText("Restarting level...", 30)/2, h/2 + 40, 30, RAYWHITE);
        EndDrawing();
        return;
    }
    if (state==CLEAR) {
        float t = loadingTimer/3.0f;
        unsigned char r = (unsigned char)(255);
        unsigned char g = (unsigned char)(215 - 55 * t);
        unsigned char b = (unsigned char)(0);
        BeginDrawing();
        ClearBackground({r, g, b, 255});
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        DrawText("CONGRATULATIONS!",
            w/2 - MeasureText("CONGRATULATIONS!", 60)/2,
            h/2 - 80, 60, BLACK);
        DrawText("GAME CLEARED!",
            w/2 - MeasureText("GAME CLEARED!", 50)/2,
            h/2, 50, BLACK);
        EndDrawing();
        return;
    }
    BeginDrawing();
    ClearBackground({10, 10, 30, 255});
    Camera3D cam = player.getCamera();
    BeginMode3D(cam);
    for (auto&s : stars) {
        float bri = (sinf(s.twinkles * 5.0f) + 1.0f) / 2.0f;
        Color c = {
            (unsigned char)(200 + 55 * bri),
            (unsigned char)(200 + 55 * bri),
            (unsigned char)(200 + 55 * bri),
            255
        };
        DrawSphere(s.position, s.size, c);
    }
    float cellSize = maze->getCellSize();
    float totalSize = maze->getColumn() * cellSize;
    DrawModel(grassModel, {
        totalSize / 2.0f,
        0.0f,
        totalSize / 2.0f
    }, 1.0f, WHITE);
    maze->Draw3D(wallModel);
    for (auto& g : ghosts) g.Draw(ghost, cam);
    EndMode3D();
    DrawText(TextFormat("LEVEL: %d", level), 20, 20, 20, DARKGRAY);
    EndDrawing();
}

Game::~Game() {
    delete maze;
    delete playBtn;
    delete tutorBtn;
    delete exitBtn;
    delete backBtn;
    UnloadModel(wallModel);
    UnloadModel(grassModel);
    UnloadTexture(grass);
    UnloadTexture(ghost);
    UnloadTexture(menuBg);
    UnloadMusicStream(permBgSound);
    UnloadMusicStream(menuSound);
    CloseAudioDevice();
    UnloadSound(exit);
    UnloadSound(jumpscare);
}