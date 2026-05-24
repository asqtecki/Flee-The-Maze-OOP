#pragma once

#include <raylib.h>

class Button {
    Vector2 position;
    Texture2D texture; //to store the path
    int width, height; 
    float scale; //for resizing
    public:
        Button(const char* path, Vector2 pos, float s);
        void Draw() const;
        bool isClicked(Vector2 pos, bool isPressed) const;
        ~Button();
};