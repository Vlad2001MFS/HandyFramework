#pragma once
#include "../IO/hdStream.hpp"
#include "../Core/hdHandle.hpp"

namespace hd {

using HSound = Handle<struct SoundImpl*, struct TAG_SoundImpl>;
using HSoundChannel = Handle<int, struct TAG_SoundChannelImpl, -1>;
using HMusic = Handle<struct MusicImpl*, struct TAG_MusicImpl>;

class SoundContext {
    HD_NONCOPYABLE_CLASS(SoundContext)
public:
    SoundContext();
    SoundContext(uint32_t freq, uint32_t chunkSize, bool stereo);
    ~SoundContext();

    void create(uint32_t freq, uint32_t chunkSize, bool stereo);
    void destroy();

    HSound createSoundFromStream(StreamReader &stream);
    HSound createSoundFromFile(const std::string &filename);
    void destroySound(HSound &handle);
    HSoundChannel playSound(const HSoundChannel &channel, const HSound &handle, int loops);
    void pauseSound(const HSoundChannel &channel);
    void resumeSound(const HSoundChannel &channel);
    void stopSound(const HSoundChannel &channel);
    bool isPlayingSound(const HSoundChannel &channel) const;
    bool isPausedSound(const HSoundChannel &channel) const;

    HMusic createMusicFromStream(StreamReader &stream);
    HMusic createMusicFromFile(const std::string &filename);
    void destroyMusic(HMusic &handle);
    void playMusic(const HMusic &handle, int loops);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();
    bool isPlayingMusic() const;
    bool isPausedMusic() const;

private:
    std::vector<HSound> mSounds;
    std::vector<HMusic> mMusics;
};

}
