#include "SDLPlayer.h"
#include <iostream>

SDLPlayer& SDLPlayer::Instance() {
    static SDLPlayer instance;
    return instance;
}

bool SDLPlayer::init() {
    if (!initialized) {
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
            MessageBoxA(NULL, SDL_GetError(), "SDL Init Error", MB_OK);
            return false;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            MessageBoxA(NULL, Mix_GetError(), "SDL_mixer Init Error", MB_OK);
            return false;
        }
        initialized = true;
    }
    return true;
}

void SDLPlayer::shutdown() {
    if (initialized) {
        stopMusic();
        Mix_CloseAudio();
        SDL_Quit();
        initialized = false;
    }
}

bool SDLPlayer::playMP3Async(const std::string& file, int loops) {
    if (!initialized) return false;

    // Si ya se está reproduciendo música, no hacer nada
    if (Mix_PlayingMusic()) {
        return false; // música ya en reproducción
    }

    if (currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }

    currentMusic = Mix_LoadMUS(file.c_str());
    if (!currentMusic) {
        MessageBoxA(NULL, Mix_GetError(), "Error loading MP3", MB_OK);
        return false;
    }

    if (Mix_PlayMusic(currentMusic, loops) == -1) {
        MessageBoxA(NULL, Mix_GetError(), "Error playing MP3", MB_OK);
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
        return false;
    }

    return true;
}

void SDLPlayer::stopMusic() {
    if (currentMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    playlist.clear();
    loopPlaylist = false;
}

void SDLPlayer::playPlaylistShuffle(const std::vector<std::string>& files, bool loop, int maxDurationMs) {
    if (!initialized || files.empty()) return;

    stopMusic();

    playlist = files;       // banco de sonidos
    loopPlaylist = loop;
    this->maxDurationMs = maxDurationMs;
    startTime = SDL_GetTicks();

    // Configurar callback
    Mix_HookMusicFinished(SDLPlayer::musicFinishedCallback);

    // Reproducir el primer sonido aleatorio
    playNextInPlaylist();
}

void SDLPlayer::playNextInPlaylist() {
    if (playlist.empty()) return;

    // Revisar duración total
    if (maxDurationMs > 0) {
        Uint32 elapsed = SDL_GetTicks() - startTime;
        if ((int)elapsed >= maxDurationMs) {
            printf("[SDLPlayer] Tiempo máximo alcanzado, fadeout y stop\n");
            Mix_FadeOutMusic(300);
            return;
        }
    }

    // Elegir sonido aleatorio
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> dist(0, playlist.size() - 1);
    size_t idx = dist(g);
    std::string file = playlist[idx];

    printf("[SDLPlayer] Reproduciendo: %s\n", file.c_str());

    currentMusic = Mix_LoadMUS(file.c_str());
    if (!currentMusic) {
        std::cerr << "[SDLPlayer] Error al cargar MP3: " << file << "\n";
        playNextInPlaylist();
        return;
    }

    if (Mix_PlayMusic(currentMusic, 0) == -1) {
        std::cerr << "[SDLPlayer] Error al reproducir MP3: " << file << "\n";
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
        playNextInPlaylist();
        return;
    }
}

void SDLPlayer::musicFinishedCallback() {
    auto& inst = SDLPlayer::Instance();

    if (inst.maxDurationMs > 0) {
        Uint32 elapsed = SDL_GetTicks() - inst.startTime;
        if ((int)elapsed >= inst.maxDurationMs) {
            printf("[SDLPlayer] Tiempo máximo alcanzado, fadeout y stop\n");
            Mix_FadeOutMusic(300);
            return;
        }
    }

    // Reproducir siguiente sonido aleatorio
    inst.playNextInPlaylist();
}
