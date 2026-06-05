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

    // Initialisation avec l'AssetManager Android
    bool init(AAssetManager* assetManager);
    void shutdown();

    // Charge un son (effet court) depuis un fichier dans les assets
    bool loadSound(const std::string& name, const std::string& filename);
    
    // Joue un effet sonore une fois
    bool play(const std::string& soundName);
    
    // Joue une musique (possible en boucle)
    bool playMusic(const std::string& soundName, bool loop = false);
    
    // Arrête une musique
    bool stopMusic(const std::string& soundName);
    
    // Libère les ressources d'un son
    void unload(const std::string& soundName);

private:
    Sound();

    // Décodage d'un fichier WAV (PCM 16 bits)
    bool decodeWAV(const std::string& filename, std::vector<short>& outSamples,
                   SLuint32& sampleRate, SLuint32& channels);

    // Structure interne pour stocker un son
    struct SoundData {
        std::vector<short> samples;
        SLuint32 sampleRate;
        SLuint32 channels;
        SLObjectItf playerObj;
        SLPlayItf playerPlay;
        SLAndroidSimpleBufferQueueItf bufferQueue;
        bool isPlaying;
    };

    std::unordered_map<std::string, SoundData> mSounds;
    AAssetManager* mAssetManager;
    bool mInitialized;

    // Objets OpenSL ES principaux
    SLObjectItf mEngineObj;
    SLEngineItf mEngine;
    SLObjectItf mOutputMixObj;
};
