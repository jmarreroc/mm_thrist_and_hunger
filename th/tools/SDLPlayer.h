#pragma once
#include <windows.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

class SDLPlayer {
public:
    static SDLPlayer& Instance();

    bool init();
    void shutdown();

    // Reproducción de un solo MP3
    bool playMP3Async(const std::string& file, int loops = 0);
    void stopMusic();

    // Nueva función: shuffle playlist
    void playPlaylistShuffle(const std::vector<std::string>& files, bool loop, int maxDurationMs);

    SDLPlayer(const SDLPlayer&) = delete;
    SDLPlayer& operator=(const SDLPlayer&) = delete;

private:
    SDLPlayer() = default;
    ~SDLPlayer() = default;

    bool initialized = false;
    Mix_Music* currentMusic = nullptr;

    // Para playlist shuffle
    std::vector<std::string> playlist;
    bool loopPlaylist = false;

    // En tu clase SDLPlayer (añade estos miembros privados)
    Uint32 startTime = 0;
    int maxDurationMs = -1; // -1 = sin límite


    static void musicFinishedCallback();
    void playNextInPlaylist();
};