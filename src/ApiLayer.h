#pragma once

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace Chip8Emu
{

// Responsible for rendering and input handling
class ApiLayer final
{
public:
    ApiLayer(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~ApiLayer();

    void Update(const void* buffer, int pitch); // pitch is equal to the row width.
    bool ProcessInput(unsigned char* keys);

private:
    SDL_Window*   window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture*  texture = nullptr;
};

} // namespace Chip8Emu