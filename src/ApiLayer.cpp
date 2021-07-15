#include "ApiLayer.h"

#include <SDL2/SDL.h>

namespace Chip8Emu
{

ApiLayer::ApiLayer(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

ApiLayer::~ApiLayer()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ApiLayer::Update(const void* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool ApiLayer::ProcessInput(unsigned char* keys)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        const unsigned char isKeyDown = event.type == SDL_KEYDOWN;
        switch (event.type)
        {
        case SDL_KEYDOWN: // Intended fall through.
        case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_x:
                    {
                        keys[0] = isKeyDown;
                        break;
                    }
                case SDLK_1:
                    {
                        keys[1] = isKeyDown;
                        break;
                    }
                case SDLK_2:
                    {
                        keys[2] = isKeyDown;
                        break;
                    }
                case SDLK_3:
                    {
                        keys[3] = isKeyDown;
                        break;
                    }
                case SDLK_q:
                    {
                        keys[4] = isKeyDown;
                        break;
                    }
                case SDLK_w:
                    {
                        keys[5] = isKeyDown;
                        break;
                    }
                case SDLK_e:
                    {
                        keys[6] = isKeyDown;
                        break;
                    }
                case SDLK_a:
                    {
                        keys[7] = isKeyDown;
                        break;
                    }
                case SDLK_s:
                    {
                        keys[8] = isKeyDown;
                        break;
                    }
                case SDLK_d:
                    {
                        keys[9] = isKeyDown;
                        break;
                    }
                case SDLK_z:
                    {
                        keys[10] = isKeyDown;
                        break;
                    }
                case SDLK_c:
                    {
                        keys[11] = isKeyDown;
                        break;
                    }
                case SDLK_4:
                    {
                        keys[12] = isKeyDown;
                        break;
                    }
                case SDLK_r:
                    {
                        keys[13] = isKeyDown;
                        break;
                    }
                case SDLK_f:
                    {
                        keys[14] = isKeyDown;
                        break;
                    }
                case SDLK_v:
                    {
                        keys[15] = isKeyDown;
                        break;
                    }
                case SDLK_ESCAPE:
                    {
                        return true; // No need to procees other events since we are done anyway.
                    }
                default:
                    break;
                }
                break;
            }
        case SDL_QUIT:
            {
                return true; // No need to procees other events since we are done anyway.
            }
        default:
            break;
        }
    }

    return false;
}

} // namespace Chip8Emu