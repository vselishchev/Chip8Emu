#include "Chip8.h"
#include "ApiLayer.h"

#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " ROMPath <Scale> <PrefferedFrameTime>(milliseconds)\n";
        return EXIT_FAILURE;
    }

    const char* romPath = argv[1];
    const int scale  = argc > 2 ? std::stoi(argv[2]) : 10;
    const float frameTime = argc > 3 ? std::stof(argv[3]) : 3;

    Chip8Emu::ApiLayer apiLayer("Chip8 Emulator", 
        Chip8Emu::VideoWidth * scale, Chip8Emu::VideoHeight * scale,
        Chip8Emu::VideoWidth, Chip8Emu::VideoHeight);

    Chip8Emu::Chip8 chip8;
    chip8.LoadROM(romPath);

    const int videoPitch = sizeof(chip8.GetVideoMemory()[0]) * Chip8Emu::VideoWidth;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!apiLayer.ProcessInput(chip8.GetKeyPad()))
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTime).count();
        if (dt > frameTime)
        {
            lastTime = currentTime;
            chip8.Cycle();

            apiLayer.Update(chip8.GetVideoMemory(), videoPitch);
        }
    }
    
    return 0;
}