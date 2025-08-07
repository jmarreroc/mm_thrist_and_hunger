// KeyMapManager.cpp
#include "KeyMapManager.h"
#include <fstream>
#include <sstream>

bool KeyMapManager::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    bool insideKeyMapping = false;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue; // Skip comments

        if (line == "[KeyMapping]") {
            insideKeyMapping = true;
            continue;
        }

        if (insideKeyMapping) {
            if (line[0] == '[') break; // Finished reading keymapping section

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);

            try {
                int keyCode = std::stoi(val);
                keyMap[key] = keyCode;
            }
            catch (...) {
                continue;
            }
        }
    }

    return true;
}

int KeyMapManager::GetKeyCode(const std::string& action) const {
    auto it = keyMap.find(action);
    if (it != keyMap.end()) return it->second;
    return -1;
}