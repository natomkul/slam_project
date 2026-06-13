#include "NavCommandParser.h"

#include <cctype>
#include <cstdlib>
#include <string>

namespace
{
std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
    {
        start++;
    }

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
    {
        end--;
    }

    return s.substr(start, end - start);
}
}

bool parseNavCommandLine(const std::string& line, RequestWire& out)
{
    const std::string t = trim(line);
    if (t.empty())
    {
        return false;
    }

    const size_t comma = t.find(',');
    if (comma == std::string::npos)
    {
        return false;
    }

    const std::string verb = trim(t.substr(0, comma));
    const std::string valueStr = trim(t.substr(comma + 1));
    const float value = std::strtof(valueStr.c_str(), nullptr);

    if (verb == "move")
    {
        out.type = RequestType::move;
        out.distanceInM = value;
        out.rotationAngle = 0.f;
        return true;
    }

    if (verb == "rotate")
    {
        out.type = RequestType::rotate;
        out.distanceInM = 0.f;
        out.rotationAngle = value;
        return true;
    }

    return false;
}
