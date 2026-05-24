#include "../include/Button.hpp"

Button::Button(const char* path, Vector2 pos, float s) : scale(s) {
    Image img = LoadImage(path);
    width = static_cast<int>(img.width * scale);
    height = static_cast<int>(img.height * scale);
    ImageResize(&img, width, height);
    texture = LoadTextureFromImage(img);
    position = pos;
}

void Button::Draw() const {
    DrawTextureV(texture, position, WHITE);
}

bool Button::isClicked(Vector2 pos, bool isPressed) const {
    Rectangle rect{position.x, position.y, (float)width, (float)height};
    return CheckCollisionPointRec(pos, rect) && isPressed;
}

Button::~Button() {
    UnloadTexture(texture);
}