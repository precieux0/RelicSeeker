#include "sound.h"
#include "utils.h"
#include <cstring>
#include <fstream>

static Sound* sInstance = nullptr;

Sound& Sound::get() {
    if (!sInstance) sInstance = new Sound();
    return *sInstance;
}

Sound::Sound() : mAssetManager(nullptr), mInitialized(false),
                 mEngineObj(nullptr), mEngine(nullptr), mOutputMixObj(nullptr) {}

Sound::~Sound() {
    shutdown();
    if (sInstance == this) sInstance = nullptr;
}

bool Sound::init(AAssetManager* assetManager) {
    if (mInitialized) return true;
    mAssetManager = assetManager;

    SLresult result;

    // Création du moteur OpenSL ES
    result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Sound: slCreateEngine failed (%d)", result);
        return false;
    }

    result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Sound: Engine Realize failed (%d)", result);
        return false;
    }

    result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Sound: Engine GetInterface failed (%d)", result);
        return false;
    }

    // Création du mixeur de sortie
    result = (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Sound: CreateOutputMix failed (%d)", result);
        return false;
    }

    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Sound: OutputMix Realize failed (%d)", result);
        return false;
    }

    mInitialized = true;
    LOGI("Sound: OpenSL ES initialized successfully");
    return true;
}

void Sound::shutdown() {
    // Détruire tous les players
    for (auto& pair : mSounds) {
        if (pair.second.playerObj) {
            (*pair.second.playerObj)->Destroy(pair.second.playerObj);
        }
    }
    mSounds.clear();

    if (mOutputMixObj) {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = nullptr;
    }
    if (mEngineObj) {
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
    }
    mEngine = nullptr;
    mInitialized = false;
    LOGI("Sound: Shutdown complete");
}

bool Sound::decodeWAV(const std::string& filename, std::vector<short>& outSamples,
                      SLuint32& outSampleRate, SLuint32& outChannels) {
    if (!mAssetManager) {
        LOGE("decodeWAV: AssetManager non initialisé");
        return false;
    }

    AAsset* asset = AAssetManager_open(mAssetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if (!asset) {
        LOGE("decodeWAV: impossible d'ouvrir %s", filename.c_str());
        return false;
    }

    off_t fileSize = AAsset_getLength(asset);
    if (fileSize < 44) {
        LOGE("decodeWAV: fichier trop petit (%lld bytes)", (long long)fileSize);
        AAsset_close(asset);
        return false;
    }

    // Lire l'en-tête WAV
    char header[44];
    if (AAsset_read(asset, header, 44) != 44) {
        LOGE("decodeWAV: échec de lecture de l'en-tête");
        AAsset_close(asset);
        return false;
    }

    // Vérifier le format "WAVE"
    if (memcmp(header + 8, "WAVE", 4) != 0) {
        LOGE("decodeWAV: format non WAVE");
        AAsset_close(asset);
        return false;
    }
    if (memcmp(header + 12, "fmt ", 4) != 0) {
        LOGE("decodeWAV: chunk fmt manquant");
        AAsset_close(asset);
        return false;
    }

    uint16_t audioFormat = *reinterpret_cast<uint16_t*>(header + 20);
    if (audioFormat != 1) {
        LOGE("decodeWAV: format audio non PCM (format %d)", audioFormat);
        AAsset_close(asset);
        return false;
    }

    outChannels = *reinterpret_cast<uint16_t*>(header + 22);
    outSampleRate = *reinterpret_cast<uint32_t*>(header + 24);
    uint16_t bitsPerSample = *reinterpret_cast<uint16_t*>(header + 34);
    if (bitsPerSample != 16) {
        LOGE("decodeWAV: seulement du PCM 16 bits est supporté (bits: %d)", bitsPerSample);
        AAsset_close(asset);
        return false;
    }

    // Chercher le chunk "data"
    off_t dataPos = 44;
    uint32_t dataSize = 0;
    bool dataFound = false;

    while (dataPos + 8 <= fileSize) {
        AAsset_seek(asset, dataPos, SEEK_SET);
        char chunkId[5] = {0};
        if (AAsset_read(asset, chunkId, 4) != 4) break;
        uint32_t chunkSize;
        if (AAsset_read(asset, &chunkSize, 4) != 4) break;

        if (memcmp(chunkId, "data", 4) == 0) {
            dataSize = chunkSize;
            dataFound = true;
            dataPos += 8;
            break;
        }
        dataPos += 8 + chunkSize;
    }

    if (!dataFound) {
        LOGE("decodeWAV: chunk data non trouvé");
        AAsset_close(asset);
        return false;
    }

    // Lire les données audio
    AAsset_seek(asset, dataPos, SEEK_SET);
    size_t sampleCount = dataSize / 2;
    outSamples.resize(sampleCount);
    if (AAsset_read(asset, outSamples.data(), dataSize) != (ssize_t)dataSize) {
        LOGE("decodeWAV: échec de lecture des données audio");
        AAsset_close(asset);
        return false;
    }

    AAsset_close(asset);
    LOGI("decodeWAV: %s, %d samples, %d Hz, %d canaux", filename.c_str(),
         (int)sampleCount, outSampleRate, outChannels);
    return true;
}

bool Sound::loadSound(const std::string& name, const std::string& filename) {
    if (!mInitialized) {
        LOGE("loadSound: Sound system not initialized");
        return false;
    }

    if (mSounds.find(name) != mSounds.end()) {
        LOGI("loadSound: Sound '%s' already loaded", name.c_str());
        return true;
    }

    SoundData data;
    if (!decodeWAV(filename, data.samples, data.sampleRate, data.channels)) {
        LOGE("loadSound: Échec du décodage WAV %s", filename.c_str());
        return false;
    }

    // Configurer le data source (buffer queue)
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
        SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1
    };
    SLDataFormat_PCM format_pcm = {
        SL_DATAFORMAT_PCM,
        data.channels,
        data.sampleRate * 1000,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        (data.channels == 2) ? (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT) : SL_SPEAKER_FRONT_CENTER,
        SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // Data sink (output mix)
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
    SLDataSink audioSnk = {&loc_outmix, nullptr};

    // Interfaces nécessaires
    const SLInterfaceID ids[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result = (*mEngine)->CreateAudioPlayer(mEngine, &data.playerObj, &audioSrc, &audioSnk, 2, ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("loadSound: CreateAudioPlayer failed (%d)", result);
        return false;
    }

    result = (*data.playerObj)->Realize(data.playerObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("loadSound: Realize failed (%d)", result);
        (*data.playerObj)->Destroy(data.playerObj);
        return false;
    }

    result = (*data.playerObj)->GetInterface(data.playerObj, SL_IID_PLAY, &data.playerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("loadSound: GetInterface PLAY failed (%d)", result);
        (*data.playerObj)->Destroy(data.playerObj);
        return false;
    }

    result = (*data.playerObj)->GetInterface(data.playerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &data.bufferQueue);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("loadSound: GetInterface BUFFERQUEUE failed (%d)", result);
        (*data.playerObj)->Destroy(data.playerObj);
        return false;
    }

    // Envoyer les données
    result = (*data.bufferQueue)->Enqueue(data.bufferQueue, data.samples.data(), data.samples.size() * sizeof(short));
    if (result != SL_RESULT_SUCCESS) {
        LOGE("loadSound: Enqueue failed (%d)", result);
        (*data.playerObj)->Destroy(data.playerObj);
        return false;
    }

    data.isPlaying = false;
    mSounds[name] = std::move(data);
    LOGI("loadSound: Son '%s' chargé avec succès", name.c_str());
    return true;
}

bool Sound::play(const std::string& soundName) {
    auto it = mSounds.find(soundName);
    if (it == mSounds.end()) {
        LOGE("play: Son '%s' non trouvé", soundName.c_str());
        return false;
    }
    SoundData& data = it->second;
    // Re-enqueue au cas où le buffer a été consommé
    (*data.bufferQueue)->Clear(data.bufferQueue);
    SLresult result = (*data.bufferQueue)->Enqueue(data.bufferQueue, data.samples.data(), data.samples.size() * sizeof(short));
    if (result != SL_RESULT_SUCCESS) {
        LOGE("play: Enqueue failed (%d)", result);
        return false;
    }
    result = (*data.playerPlay)->SetPlayState(data.playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("play: SetPlayState failed (%d)", result);
        return false;
    }
    return true;
}

// Callback pour la boucle musicale
static void musicLoopCallback(SLAndroidSimpleBufferQueueItf bq, void* context) {
    Sound::SoundData* data = (Sound::SoundData*)context;
    if (data) {
        (*bq)->Enqueue(bq, data->samples.data(), data->samples.size() * sizeof(short));
    }
}

bool Sound::playMusic(const std::string& soundName, bool loop) {
    auto it = mSounds.find(soundName);
    if (it == mSounds.end()) {
        LOGE("playMusic: Son '%s' non trouvé", soundName.c_str());
        return false;
    }
    SoundData& data = it->second;
    // Arrêter si déjà en lecture
    (*data.playerPlay)->SetPlayState(data.playerPlay, SL_PLAYSTATE_STOPPED);
    (*data.bufferQueue)->Clear(data.bufferQueue);
    SLresult result = (*data.bufferQueue)->Enqueue(data.bufferQueue, data.samples.data(), data.samples.size() * sizeof(short));
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playMusic: Enqueue failed (%d)", result);
        return false;
    }
    if (loop) {
        result = (*data.bufferQueue)->RegisterCallback(data.bufferQueue, musicLoopCallback, &data);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("playMusic: RegisterCallback failed (%d)", result);
        }
    }
    result = (*data.playerPlay)->SetPlayState(data.playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playMusic: SetPlayState failed (%d)", result);
        return false;
    }
    data.isPlaying = true;
    return true;
}

bool Sound::stopMusic(const std::string& soundName) {
    auto it = mSounds.find(soundName);
    if (it == mSounds.end()) return false;
    SoundData& data = it->second;
    (*data.playerPlay)->SetPlayState(data.playerPlay, SL_PLAYSTATE_STOPPED);
    (*data.bufferQueue)->Clear(data.bufferQueue);
    data.isPlaying = false;
    return true;
}

void Sound::unload(const std::string& soundName) {
    auto it = mSounds.find(soundName);
    if (it != mSounds.end()) {
        if (it->second.playerObj) {
            (*it->second.playerObj)->Destroy(it->second.playerObj);
        }
        mSounds.erase(it);
        LOGI("unload: Son '%s' déchargé", soundName.c_str());
    }
}
