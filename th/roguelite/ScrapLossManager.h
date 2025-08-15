#pragma once

class ScrapLossManager {
public:
    static ScrapLossManager& Instance();

    void Update(float dt);

private:
    ScrapLossManager() = default;
    ScrapLossManager(const ScrapLossManager&) = delete;
    ScrapLossManager& operator=(const ScrapLossManager&) = delete;

    bool diedRun = false;
    bool diedLoad = false;

};