#pragma once
#include <string>
#include <unordered_map>

class KeyMapManager {
public:
    bool LoadFromFile(const std::string& path);
    int GetKeyCode(const std::string& action) const;

private:
    std::unordered_map<std::string, int> keyMap;
};