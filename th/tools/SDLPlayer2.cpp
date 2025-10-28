#include "SDLPlayer2.h"
#include "th/Config.h"
#include <iostream>

SDLPlayer2& SDLPlayer2::Instance() {
    static SDLPlayer2 instance;
    return instance;
}


void SDLPlayer2::setGlobalVolume(int volume) {
    printf("[SDLPlayer2] setting volume %d...\n", volume);

    Mix_Volume(-1, volume);
}


bool SDLPlayer2::init() {
    if (!initialized) {
        const auto& cfg = Config::instance();

        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
            MessageBoxA(NULL, SDL_GetError(), "SDL Init Error", MB_OK);
            return false;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            MessageBoxA(NULL, Mix_GetError(), "SDL_mixer Init Error", MB_OK);
            return false;
        }

        Mix_ChannelFinished(SDLPlayer2::channelFinishedCallback);
        setGlobalVolume(cfg.sanity().volume_modifier * MIX_MAX_VOLUME);

        initialized = true;
    }
    return true;
}

void SDLPlayer2::shutdown() {
    if (initialized) {
        stopMusic();

        for (auto& pair : soundCache) {
            Mix_FreeChunk(pair.second);
        }

        soundCache.clear();
        Mix_CloseAudio();
        SDL_Quit();
        initialized = false;
    }
}

Mix_Chunk* SDLPlayer2::getOrLoadChunk(const std::string& file) {
    auto it = soundCache.find(file);
    if (it != soundCache.end())
        return it->second;

    Mix_Chunk* chunk = Mix_LoadWAV(file.c_str());
    if (!chunk) {
        std::cerr << "[SDLPlayer2] Error al cargar WAV: " << file << "\n";
        return nullptr;
    }

    soundCache[file] = chunk;
    return chunk;
}

bool SDLPlayer2::playMP3Async(const std::string& file, int loops) {
    if (!initialized)
        return false;

    if (Mix_Playing(-1))
        return false;

    Mix_Chunk* chunk = getOrLoadChunk(file);
    if (!chunk)
        return false;

    int channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel == -1) {
        std::cerr << "[SDLPlayer2] Error al reproducir: " << file << "\n";
        return false;
    }

    return true;
}

void SDLPlayer2::stopMusic() {
    Mix_HaltChannel(-1);
    playlist.clear();
    loopPlaylist = false;
}

void SDLPlayer2::playPlaylistShuffle(const std::vector<std::string>& files, bool loop, int maxDurationMs) {
    if (!initialized || files.empty())
        return;

    stopMusic();
    playlist = files;
    loopPlaylist = loop;
    this->maxDurationMs = maxDurationMs;
    startTime = SDL_GetTicks();
    playNextInPlaylist();
}

void SDLPlayer2::playNextInPlaylist() {
    if (playlist.empty())
        return;

    if (maxDurationMs > 0) {
        Uint32 elapsed = SDL_GetTicks() - startTime;
        if ((int)elapsed >= maxDurationMs) {
            std::cout << "[SDLPlayer2] Tiempo máximo alcanzado\n";
            return;
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> dist(0, playlist.size() - 1);

    std::string file = playlist[dist(g)];
    Mix_Chunk* chunk = getOrLoadChunk(file);
    if (!chunk) {
        std::cerr << "[SDLPlayer2] Error al cargar: " << file << "\n";
        playNextInPlaylist();
        return;
    }

    int channel = Mix_PlayChannel(-1, chunk, 0);
    if (channel == -1) {
        std::cerr << "[SDLPlayer2] Error al reproducir: " << file << "\n";
        playNextInPlaylist();
        return;
    }
}

void SDLPlayer2::channelFinishedCallback(int channel) {
    auto& inst = SDLPlayer2::Instance();
    if (inst.loopPlaylist) {
        Uint32 elapsed = SDL_GetTicks() - inst.startTime;
        if (inst.maxDurationMs > 0 && (int)elapsed >= inst.maxDurationMs)
            return;
        inst.playNextInPlaylist();
    }
}