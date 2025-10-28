#pragma once
#include <windows.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <unordered_map>

class SDLPlayer2 {
public:
    static SDLPlayer2& Instance();

    bool init();
    void shutdown();

    void setGlobalVolume(int volume);
    bool playMP3Async(const std::string& file, int loops = 0);
    void stopMusic();

    void playPlaylistShuffle(const std::vector<std::string>& files, bool loop, int maxDurationMs);

    SDLPlayer2(const SDLPlayer2&) = delete;
    SDLPlayer2& operator=(const SDLPlayer2&) = delete;

private:
    SDLPlayer2() = default;
    ~SDLPlayer2() = default;

    bool initialized = false;

    // Cache de efectos
    std::unordered_map<std::string, Mix_Chunk*> soundCache;

    // Playlist shuffle
    std::vector<std::string> playlist;
    bool loopPlaylist = false;
    Uint32 startTime = 0;
    int maxDurationMs = -1;

    static void channelFinishedCallback(int channel);
    void playNextInPlaylist();

    Mix_Chunk* getOrLoadChunk(const std::string& file);
};
