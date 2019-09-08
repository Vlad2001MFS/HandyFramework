#include "hdSoundContext.hpp"
#include "../IO/hdFileStream.hpp"
#include "../Core/hdLog.hpp"
#include "SDL2/SDL_mixer.h"
#include <memory>
#include <algorithm>

namespace hd {

struct SoundImpl {
    std::string name;
    std::unique_ptr<uint8_t[]> buf;
    Mix_Chunk *chunk;
};

struct MusicImpl {
    std::string name;
    std::unique_ptr<uint8_t[]> buf;
    Mix_Music *music;
};

SoundContext::SoundContext(uint32_t freq, uint32_t chunkSize, bool stereo) {
    create(freq, chunkSize, stereo);
}

SoundContext::~SoundContext() {
    destroy();
}

void SoundContext::create(uint32_t freq, uint32_t chunkSize, bool stereo) {
    destroy();
    if (Mix_Init(0) != 0) {
        HD_LOG_ERROR("Failed to initialize SDL_Mixer. Errors: %s", Mix_GetError());
    }
    if (Mix_OpenAudio(static_cast<int>(freq), AUDIO_S16SYS, stereo ? 2 : 1, static_cast<int>(chunkSize)) != 0) {
        HD_LOG_ERROR("Failed to initialize SDL_Mixer audio device. Errors: %s", Mix_GetError());
    }
}

void SoundContext::destroy() {
    for (auto &it : mSounds) {
        destroySound(it);
    }
    for (auto &it : mMusics) {
        destroyMusic(it);
    }
    Mix_CloseAudio();
    Mix_Quit();
}

HSound SoundContext::createSoundFromStream(Stream &stream) {
    HD_ASSERT(stream.isReadable());
    SoundImpl *impl = new SoundImpl();
    impl->name = stream.getName();
    mSounds.push_back(HSound(impl));
    size_t bufSize = stream.getSize();
    impl->buf = std::make_unique<uint8_t[]>(bufSize);
    if (stream.read(impl->buf.get(), bufSize) != bufSize) {
        HD_LOG_ERROR("Failed to load sound from stream '%s'", stream.getName().data());
    }
    SDL_RWops *rwops = SDL_RWFromConstMem(impl->buf.get(), static_cast<int>(bufSize));
    if (!rwops) {
        HD_LOG_ERROR("Failed to create SDL_RWops from constant memory. Errors: %s", SDL_GetError());
    }
    impl->chunk = Mix_LoadWAV_RW(rwops, true);
    if (!impl->chunk) {
        HD_LOG_ERROR("Failed to create sound from stream '%s'", stream.getName().data());
    }
    return mSounds.back();
}

HSound SoundContext::createSoundFromFile(const std::string &filename) {
    FileStream fs(filename, FileMode::Read);
    return createSoundFromStream(fs);
}

void SoundContext::destroySound(HSound &handle) {
    if (handle) {
        mSounds.erase(std::remove(mSounds.begin(), mSounds.end(), handle), mSounds.end());
        Mix_FreeChunk(handle->chunk);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

HSoundChannel SoundContext::playSound(const HSoundChannel &channel, const HSound &handle, int loops) {
    HD_ASSERT(handle);
    int playedChannel = Mix_PlayChannel(channel.value, handle->chunk, loops);
    if (playedChannel == -1) {
        HD_LOG_ERROR("Failed to play sound '%s'. Errors: %s", handle->name.data(), Mix_GetError());
    }
    return HSoundChannel(playedChannel);
}

void SoundContext::pauseSound(const HSoundChannel &channel) {
    HD_ASSERT(channel);
    Mix_Pause(channel.value);
}

void SoundContext::resumeSound(const HSoundChannel &channel) {
    HD_ASSERT(channel);
    Mix_Resume(channel.value);
}

void SoundContext::stopSound(const HSoundChannel &channel) {
    HD_ASSERT(channel);
    Mix_HaltChannel(channel.value);
}

bool SoundContext::isPlayingSound(const HSoundChannel &channel) const {
    HD_ASSERT(channel);
    return Mix_Playing(channel.value);
}

bool SoundContext::isPausedSound(const HSoundChannel &channel) const {
    HD_ASSERT(channel);
    return Mix_Paused(channel.value);
}

HMusic SoundContext::createMusicFromStream(Stream &stream) {
    HD_ASSERT(stream.isReadable());
    MusicImpl *impl = new MusicImpl();
    impl->name = stream.getName();
    mMusics.push_back(HMusic(impl));
    size_t bufSize = stream.getSize();
    impl->buf = std::make_unique<uint8_t[]>(bufSize);
    if (stream.read(impl->buf.get(), bufSize) != bufSize) {
        HD_LOG_ERROR("Failed to load music from stream '%s'", stream.getName().data());
    }
    SDL_RWops *rwops = SDL_RWFromConstMem(impl->buf.get(), static_cast<int>(bufSize));
    if (!rwops) {
        HD_LOG_ERROR("Failed to create SDL_RWops from constant memory. Errors: %s", SDL_GetError());
    }
    impl->music = Mix_LoadMUS_RW(rwops, true);
    if (!impl->music) {
        HD_LOG_ERROR("Failed to create music from stream '%s'", stream.getName().data());
    }
    return mMusics.back();
}

HMusic SoundContext::createMusicFromFile(const std::string &filename) {
    FileStream fs(filename, FileMode::Read);
    return createMusicFromStream(fs);
}

void SoundContext::destroyMusic(HMusic &handle) {
    if (handle) {
        mMusics.erase(std::remove(mMusics.begin(), mMusics.end(), handle), mMusics.end());
        Mix_FreeMusic(handle->music);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void SoundContext::playMusic(const HMusic &handle, int loops) {
    HD_ASSERT(handle);
    if (Mix_PlayMusic(handle->music, loops) != 0) {
        HD_LOG_ERROR("Failed to play music '%s'. Errors: %s", handle->name.data(), Mix_GetError());
    }
}

void SoundContext::pauseMusic() {
    Mix_PauseMusic();
}

void SoundContext::resumeMusic() {
    Mix_ResumeMusic();
}

void SoundContext::stopMusic() {
    Mix_HaltMusic();
}

bool SoundContext::isPlayingMusic() const {
    return Mix_PlayingMusic();
}

bool SoundContext::isPausedMusic() const {
    return Mix_PausedMusic();
}

}
