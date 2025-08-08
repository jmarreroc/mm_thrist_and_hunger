#pragma once
#include <string>
#include <unordered_map>

class KeyMapManager {
public:
    static KeyMapManager& Get();

    int GetKeyCode(const std::string& action) const;

private:
    KeyMapManager();
    KeyMapManager(const KeyMapManager&) = delete;
    KeyMapManager& operator=(const KeyMapManager&) = delete;

    bool LoadFromFile(const std::string& path);
    std::unordered_map<std::string, int> keyMap;
};