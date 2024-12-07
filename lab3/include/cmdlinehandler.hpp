#ifndef __CMDLINEHANDLER_H__
#define __CMDLINEHANDLER_H__

#include <string>
#include <optional>
#include <iostream>
#include <variant>
#include <vector>

enum class cmdType{
    CAMERA = 0,
    LIGHT,
    MOVE,
    POWER,
    QUIT
};

struct coords
{
    float theta;
    float phi;
    float r;
};

struct CMD
{
    cmdType type;
    std::optional<std::variant<coords, float, std::string, std::monostate>> cmdVerb;
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
std::optional<CMD> getUserCommand();

#endif // __CMDLINEHANDLER_H__