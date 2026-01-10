#define MINIAUDIO_IMPLEMENTATION
#include "../include/Audio.h"
#include "../include/util/Logging.h"
#include "miniaudio.h"
#include <algorithm>

namespace CPL {
ma_engine AudioManager::s_Engine;
std::unique_ptr<ma_sound> AudioManager::s_Music;
std::vector<std::unique_ptr<ma_sound>> AudioManager::s_ActiveSounds;

void AudioManager::Init() {
    if (ma_engine_init(nullptr, &s_Engine) != MA_SUCCESS) {
        Logging::Log(Logging::MessageStates::ERROR,
                     "Failed to init audio engine!");
        exit(-1);
    }
}

Audio AudioManager::LoadAudio(const std::string &audioPath) {
    return {audioPath};
}

void AudioManager::Update() {
    s_ActiveSounds.erase(std::remove_if(s_ActiveSounds.begin(),
                                        s_ActiveSounds.end(),
                                        [](auto &s) {
                                            if (!ma_sound_is_playing(s.get())) {
                                                ma_sound_uninit(s.get());
                                                return true;
                                            }
                                            return false;
                                        }),
                         s_ActiveSounds.end());
}

void AudioManager::PlaySFX(const Audio &audio) {
    auto sound = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(&s_Engine, audio.path.c_str(),
                                MA_SOUND_FLAG_DECODE, nullptr, nullptr,
                                sound.get()) != MA_SUCCESS) {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to init SFX!");
        return;
    }
    ma_sound_set_pitch(sound.get(), 1.0f);
    ma_sound_set_looping(sound.get(), MA_FALSE);
    ma_sound_start(sound.get());
    s_ActiveSounds.push_back(std::move(sound));
}

void AudioManager::PlaySFXPitch(const Audio &audio, const float pitch) {
    auto sound = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(&s_Engine, audio.path.c_str(),
                                MA_SOUND_FLAG_DECODE, nullptr, nullptr,
                                sound.get()) != MA_SUCCESS) {
        Logging::Log(Logging::MessageStates::ERROR,
                     "Failed to initialize SFX!");
        return;
    }
    ma_sound_set_pitch(sound.get(), pitch);
    ma_sound_set_looping(sound.get(), MA_FALSE);
    ma_sound_start(sound.get());
    s_ActiveSounds.push_back(std::move(sound));
}

void AudioManager::PlayMusic(const Audio &audio) {
    if (s_Music) {
        ma_sound_stop(s_Music.get());
        ma_sound_uninit(s_Music.get());
        s_Music.reset();
    }

    s_Music = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(&s_Engine, audio.path.c_str(),
                                MA_SOUND_FLAG_DECODE, nullptr, nullptr,
                                s_Music.get()) != MA_SUCCESS) {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to load music!");
        s_Music.reset();
        return;
    }

    ma_sound_set_looping(s_Music.get(), MA_TRUE);
    ma_sound_start(s_Music.get());
}

void AudioManager::PauseMusic() {
    if (s_Music)
        ma_sound_stop(s_Music.get());
}

void AudioManager::ResumeMusic() {
    if (s_Music)
        ma_sound_start(s_Music.get());
}

void AudioManager::StopMusic() {
    if (s_Music) {
        ma_sound_stop(s_Music.get());
        ma_sound_seek_to_pcm_frame(s_Music.get(), 0);
    }
}

void AudioManager::PlayMusicPitch(const Audio &audio, const float pitch) {
    if (s_Music) {
        ma_sound_stop(s_Music.get());
        ma_sound_uninit(s_Music.get());
        s_Music.reset();
    }

    s_Music = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(&s_Engine, audio.path.c_str(),
                                MA_SOUND_FLAG_DECODE, nullptr, nullptr,
                                s_Music.get()) != MA_SUCCESS) {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to load music!");
        s_Music.reset();
        return;
    }

    ma_sound_set_pitch(s_Music.get(), pitch);
    ma_sound_set_looping(s_Music.get(), MA_TRUE);
    ma_sound_start(s_Music.get());
}

void AudioManager::Close() { ma_engine_uninit(&s_Engine); }
} // namespace CPL
