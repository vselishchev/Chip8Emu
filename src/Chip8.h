#pragma once

namespace Chip8
{

constexpr unsigned int startAddress = 0x200; // Usable memory address starts only from 0x200.

class Chip8
{
public:
    Chip8();
    Chip8(const Chip8&) = delete;
    Chip8(Chip8&&) = delete;

    Chip8& operator=(const Chip8&) = delete;
    Chip8& operator=(const Chip8&&) = delete;

    void LoadROM(const char* filename);

private:
    // Instructions
    void Op00E0(); // Clear the video buffer.
    void Op00EE(); // Return from the subroutine == reduce stack pointer + set program counter to value in stack.
    void Op1nnn(); // Jump to specified address (nnn).
    void Op2nnn(); // Call subroutine == increase stack pointer + set program counter to the specified address (nnn).
    void Op3xkk(); // Skip next instruction if register Vx equal to kk.
    void Op4xkk(); // Skip next instruction if register Vx not equal to kk.
    void Op5xy0(); // Skip next instruction if register Vx equal to register Vy.

private:
    unsigned char registers[16];
    unsigned char memory[4096];
    unsigned char sp = 0; // Stack pointer;
    unsigned char delayTimer = 0;
    unsigned char soundTimer = 0;
    unsigned char keypad[16];
    unsigned short index = 0;
    unsigned short pc = startAddress; // Program counter
    unsigned short stack[16];
    unsigned short opcode = 0;
    unsigned int videoMemory[64 * 32];
};

} // namespace Chip8;