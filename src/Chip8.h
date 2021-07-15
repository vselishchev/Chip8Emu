#pragma once

namespace Chip8Emu
{

constexpr unsigned int StartAddress = 0x200; // Usable memory address starts only from 0x200.
constexpr unsigned int MemorySize = 4096;
constexpr unsigned char VideoWidth = 64u;
constexpr unsigned char VideoHeight = 32u;

class Chip8 final
{
public:
    Chip8();
    Chip8(const Chip8&) = delete;
    Chip8(Chip8&&) = delete;

    Chip8& operator=(const Chip8&) = delete;
    Chip8& operator=(const Chip8&&) = delete;

    void LoadROM(const char* filename);
    void Cycle();

    unsigned char* GetKeyPad();
    const unsigned int* GetVideoMemory() const;

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
    void OpEx9E(); // Skip next instruction if key with the value of register Vx is pressed.
    void OpExA1(); // Skip next instruction if key with the value of register Vx is not pressed.
    void OpFx07(); // Set value of register Vx to delay timer.
    void OpFx0A(); // Wait for the key press and store key index into the register Vx.
    void OpFx15(); // Set the delay timer to value of the register Vx.
    void OpFx18(); // Set the sound timer to value of the register Vx.
    void OpFx1E(); // Increment index by the value of the register Vx.
    void OpFx29(); // Set index to location of sprite for symbol stored in register Vx.
    void OpFx33(); // Takes the value from register Vx and places it into the memory in such way: stores hundreds at location "index", tens - "index + 1", digits - "Index + 2".
    void OpFx55(); // Stores the registers from V0 to Vx into the memory starting at location "index";
    void OpFx65(); // Loads the registers from V0 to Vx from the memory starting at location "index".
    void OpNull(){} // Dummy instruction in case if the opcode is wrong.

    // Redirection tables
    void Table0();
    void Table8();
    void TableE();
    void TableF();

private:
    unsigned char registers[16];
    unsigned char memory[MemorySize];
    unsigned char sp = 0; // Stack pointer;
    unsigned char delayTimer = 0;
    unsigned char soundTimer = 0;
    unsigned char keypad[16];
    unsigned short index = 0;
    unsigned short pc = StartAddress; // Program counter
    unsigned short stack[16];
    unsigned short opcode = 0;
    unsigned int videoMemory[VideoWidth * VideoHeight];

// Instead of having huge switch, we are going to implement functio table, so the opcode could lead into the function (through the indirection though.).
using Chip8Func = void(Chip8::*)();
    Chip8Func table[0xF + 1]{&Chip8::OpNull};
    Chip8Func table0[0xE + 1]{&Chip8::OpNull};
    Chip8Func table8[0xE + 1]{&Chip8::OpNull};
    Chip8Func tableE[0xE + 1]{&Chip8::OpNull};
    Chip8Func tableF[0x65 + 1]{&Chip8::OpNull};
};

} // namespace Chip8Emu