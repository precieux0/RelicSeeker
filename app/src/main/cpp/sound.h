#pragma once
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include <string>
#include <unordered_map>
#include <vector>

class Sound {
public:
    static Sound& get();
    ~Sound();
    bool init(AAssetManager* assetManager);
    void shutdown();
    bool loadSound(const std::string& name, const std::string& filename);
    bool play(const std::string& soundName);
    bool playMusic(const std::string& soundName, bool loop = false);
    bool stopMusic(const std::string& soundName);
    void stopCurrentMusic();
    bool switchMusic(const std::string& name, bool loop = true);
    void unload(const std::string& soundName);

    struct SoundData {
        std::vector<short> samples;
        SLuint32 sampleRate;
        SLuint32 channels;
        SLObjectItf playerObj;
        SLPlayItf playerPlay;
        SLAndroidSimpleBufferQueueItf bufferQueue;
        bool isPlaying;
    };

private:
    Sound();
    bool decodeWAV(const std::string& filename, std::vector<short>& outSamples,
                   SLuint32& sampleRate, SLuint32& channels);
    std::unordered_map<std::string, SoundData> mSounds;
    AAssetManager* mAssetManager;
    bool mInitialized;
    std::string mCurrentMusic;
    SLObjectItf mEngineObj;
    SLEngineItf mEngine;
    SLObjectItf mOutputMixObj;
};
