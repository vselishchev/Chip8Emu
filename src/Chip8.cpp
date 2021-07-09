#include "Chip8.h"

#include <fstream>
#include <vector>
#include <cstring>

namespace Chip8
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
    romBuffer.reserve(size);

    file.seekg(0, std::ios::beg);
    file.read(romBuffer.data(), size);
    file.close();

    std::memcpy(memory + startAddress, romBuffer.data(), size);
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

} // namespace Chip8