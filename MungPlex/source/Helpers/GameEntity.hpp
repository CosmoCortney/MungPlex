#pragma once
#include <string>

namespace MungPlex
{
    struct GameEntity
    {
        std::string Entity;
        uint64_t Location;
        std::string Datatype;
        int Size;
        bool Hex;
        std::string Value;
    };
}