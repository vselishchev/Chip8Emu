#pragma once

namespace Chip8
{

constexpr unsigned int startAddress = 0x200; // Usable memory address starts only from 0x200.
constexpr unsigned char VideoWidth = 64u;
constexpr unsigned char VideoHeight = 32u;

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
    void Op6xkk(); // Set the register Vx to the value kk;
    void Op7xkk(); // Add value to the register Vx;
    void Op8xy0(); // Set the register Vx to the value of the register Vy;
    void Op8xy1(); // Set register Vx to Vx OR Vy;
    void Op8xy2(); // Set register Vx to Vx AND Vy;
    void Op8xy3(); // Set register Vx to Vx XOR Vy;
    void Op8xy4(); // Add registers Vx and Vy with carry;
    void Op8xy5(); // Subtract register Vy from register Vx, set flag register to NOT borrow;
    void Op8xy6(); // Right shift of the register Vx. If least-significant bit is 1 - set flag register to 1;
    void Op8xy7(); // Subtract register Vx from register Vy and store in register Vx. Set flag register to NOT bororow;
    void Op8xyE(); // Left shift of the register Vx. If most-significant bit is 1 - set flag register to 1;
    void Op9xy0(); // Skip next instruction if register Vx not equal register Vy.
    void OpAnnn(); // Set index to nnn.
    void OpBnnn(); // Jump to location nnn + V0;
    void OpCxkk(); // Set register Vx to random byte and kk.
    void OpDxyn(); // Draw n-byte sprite starting at memory (Vx, Vy). Set flag register to possible collisiion.

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
    unsigned int videoMemory[VideoWidth * VideoHeight];
};

} // namespace Chip8;