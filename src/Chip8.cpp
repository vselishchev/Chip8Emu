#include "Chip8.h"

#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <random>

namespace Chip8Emu
{

constexpr unsigned int FontsetStartAddress = 50;

Chip8::Chip8()
{
    constexpr unsigned int FontsetSize = 80; // 16 symbols x 5 bytes long
    unsigned char fontset[FontsetSize] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Load fonts into the memory
    std::memcpy(memory + FontsetStartAddress, fontset, FontsetSize);

    // Define function pointer table
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::Op1nnn;
    table[0x2] = &Chip8::Op2nnn;
    table[0x3] = &Chip8::Op3xkk;
    table[0x4] = &Chip8::Op4xkk;
    table[0x5] = &Chip8::Op5xy0;
    table[0x6] = &Chip8::Op6xkk;
    table[0x7] = &Chip8::Op7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::Op9xy0;
    table[0xA] = &Chip8::OpAnnn;
    table[0xB] = &Chip8::OpBnnn;
    table[0xC] = &Chip8::OpCxkk;
    table[0xD] = &Chip8::OpDxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    table0[0x0] = &Chip8::Op00E0;
    table0[0xE] = &Chip8::Op00EE;

    table8[0x0] = &Chip8::Op8xy0;
    table8[0x1] = &Chip8::Op8xy1;
    table8[0x2] = &Chip8::Op8xy2;
    table8[0x3] = &Chip8::Op8xy3;
    table8[0x4] = &Chip8::Op8xy4;
    table8[0x5] = &Chip8::Op8xy5;
    table8[0x6] = &Chip8::Op8xy6;
    table8[0x7] = &Chip8::Op8xy7;
    table8[0xE] = &Chip8::Op8xyE;

    tableE[0x1] = &Chip8::OpExA1;
    tableE[0xE] = &Chip8::OpEx9E;

    tableF[0x07] = &Chip8::OpFx07;
    tableF[0x0A] = &Chip8::OpFx0A;
    tableF[0x15] = &Chip8::OpFx15;
    tableF[0x18] = &Chip8::OpFx18;
    tableF[0x1E] = &Chip8::OpFx1E;
    tableF[0x29] = &Chip8::OpFx29;
    tableF[0x33] = &Chip8::OpFx33;
    tableF[0x55] = &Chip8::OpFx55;
    tableF[0x65] = &Chip8::OpFx65;
}

void Chip8::LoadROM(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // Move file pointer to the end of stream to get the ROM size;
    if (!file.is_open())
    {
        return;
    }

    const std::streampos size = file.tellg();
    std::vector<char> romBuffer;
    romBuffer.resize(size);

    file.seekg(0, std::ios::beg);
    file.read(romBuffer.data(), size);
    file.close();

    std::memcpy(memory + StartAddress, romBuffer.data(), size);
}

void Chip8::Cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    (this->*(table[(opcode & 0xF000u) >> 12u]))();

    if (delayTimer > 0)
    {
        --delayTimer;
    }

    if (soundTimer > 0)
    {
        --soundTimer;
    }
}

unsigned char* Chip8::GetKeyPad()
{
    return keypad;
}

const unsigned int* Chip8::GetVideoMemory() const
{
    return videoMemory;
}

void Chip8::Op00E0() 
{
    memset(videoMemory, 0, sizeof(videoMemory));
}

void Chip8::Op00EE()
{
    --sp;
    pc = stack[sp];
}

void Chip8::Op1nnn()
{
    const unsigned short address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::Op2nnn()
{
    const unsigned short address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::Op3xkk()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u; // Get the register index;
    const unsigned char value = opcode & 0x00FFu;

    if (registers[Vx] == value)
    {
        pc += 2;
    }
}

void Chip8::Op4xkk()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u; // Get the register index;
    const unsigned char value = opcode & 0x00FFu;

    if (registers[Vx] != value)
    {
        pc += 2;
    }
}

void Chip8::Op5xy0()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u; // Get index of the register x;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u; // Get index of the register y;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::Op6xkk()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char value = opcode & 0x00FFu;

    registers[Vx] = value;
}

void Chip8::Op7xkk()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char value = opcode & 0x00FFu;

    registers[Vx] += value;
}

void Chip8::Op8xy0()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::Op8xy1()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

void Chip8::Op8xy2()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

void Chip8::Op8xy3()
{ 
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::Op8xy4()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    const unsigned short sum = registers[Vx] + registers[Vy];

    if (sum > 255u)
    {
        registers[0xFu] = 1;
    }
    else
    {
        registers[0xFu] = 0;
    }

    registers[Vx] = sum & 0xFFu;
}

void Chip8::Op8xy5()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[0xFu] = registers[Vx] > registers[Vy];

    registers[Vx] -= registers[Vy];    
}

void Chip8::Op8xy6()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
}

void Chip8::Op8xy7()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    registers[0xFu] = registers[Vy] > registers[Vx];

    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::Op8xyE()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xFu] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::Op9xy0()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::OpAnnn()
{
    const unsigned short address = opcode & 0x0FFFu;
    index = address;
}

void Chip8::OpBnnn()
{
    const unsigned short address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void Chip8::OpCxkk()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char byte = opcode & 0x00FFu;

    static std::default_random_engine randGen(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<uint8_t> randByte(0, 255u);

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OpDxyn()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char Vy = (opcode & 0x00F0u) >> 4u;
    const unsigned char height = opcode & 0x000Fu; // It's always height, since it's guaranteed that the sprite is 8 pixels(bits) wide.

    const unsigned char xPos = registers[Vx] % VideoWidth;
    const unsigned char yPos = registers[Vy] % VideoHeight;

    registers[0xF] = 0; // nullify flag register before checking for collisions.

    for (unsigned int row = 0; row < height; ++row)
    {
        const unsigned char spriteByte = memory[index + row];
        for (unsigned int column = 0; column < 8; ++column)
        {
            const unsigned char spritePixel = spriteByte & (0x80u >> column);
            unsigned int& screenPixel = videoMemory[(yPos + row) * VideoWidth + (xPos + column)];
            if (spritePixel)
            {
                if (screenPixel == 0xFFFFFFFFu) // if screen pixel is already on, set collision flag to 1;
                {
                    registers[0xF] = 1;
                }

                screenPixel ^= 0xFFFFFFFFu;
            }
        }
    }
}

void Chip8::OpEx9E()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char key = registers[Vx];
    if (keypad[key])
    {
        pc += 2;
    }
}

void Chip8::OpExA1()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char key = registers[Vx];
    if (!keypad[key])
    {
        pc += 2;
    }
}

void Chip8::OpFx07()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

void Chip8::OpFx0A()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;

    // TODO: Think about nicer way of doing this (packaging into int128?).
    if (keypad[0])
       registers[0] = 0;
    else if (keypad[1])
       registers[1] = 1;
    else if (keypad[2])
       registers[2] = 2;
    else if (keypad[3])
       registers[3] = 3;
    else if (keypad[4])
       registers[4] = 4;
    else if (keypad[5])
       registers[5] = 5;
    else if (keypad[6])
       registers[6] = 6;
    else if (keypad[7])
       registers[7] = 7;
    else if (keypad[8])
       registers[8] = 8;
    else if (keypad[9])
       registers[9] = 9;
    else if (keypad[10])
       registers[10] = 10;
    else if (keypad[11])
       registers[11] = 11;
    else if (keypad[12])
       registers[12] = 12;
    else if (keypad[13])
       registers[13] = 13;
    else if (keypad[14])
       registers[14] = 14;
    else if (keypad[15])
       registers[15] = 15;
    else
        pc -= 2;
}

void Chip8::OpFx15()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OpFx18()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OpFx1E()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    index += registers[Vx];
}

void Chip8::OpFx29()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    const unsigned char symbol = registers[Vx];

    index = FontsetStartAddress + 5 * symbol;
}

void Chip8::OpFx33()
{
    const unsigned char Vx = (opcode & 0x0F00u) >> 8u;
    unsigned char value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::OpFx55()
{
    const unsigned short Vx = (opcode & 0x0F00u) >> 8u;
    for (unsigned short i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
}

void Chip8::OpFx65()
{
    const unsigned short Vx = (opcode & 0x0F00u) >> 8u;
    for (unsigned short i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
    }
}

void Chip8::Table0()
{
    (this->*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
    (this->*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    (this->*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    (this->*(tableF[opcode & 0x00FFu]))();
}

} // namespace Chip8Emu 