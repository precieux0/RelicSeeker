#include "sound.h"
#include "utils.h"
#include <cstring>

static Sound* sInstance = nullptr;

Sound& Sound::get() {
    if (!sInstance) sInstance = new Sound();
    return *sInstance;
}

Sound::Sound() : mAssetManager(nullptr), mInitialized(false),
                 mEngineObj(nullptr), mEngine(nullptr), mOutputMixObj(nullptr) {}
Sound::~Sound() { shutdown(); }

bool Sound::init(AAssetManager* assetManager) {
    if (mInitialized) return true;
    mAssetManager = assetManager;
    SLresult result;
    result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) { LOGE("slCreateEngine failed"); return false; }
    result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { LOGE("Engine Realize failed"); return false; }
    result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine);
    if (result != SL_RESULT_SUCCESS) { LOGE("Engine GetInterface failed"); return false; }
    result = (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) { LOGE("CreateOutputMix failed"); return false; }
    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { LOGE("OutputMix Realize failed"); return false; }
    mInitialized = true;
    LOGI("OpenSL ES initialized");
    return true;
}

void Sound::shutdown() {
    for (auto& pair : mSounds) if (pair.second.playerObj) (*pair.second.playerObj)->Destroy(pair.second.playerObj);
    mSounds.clear();
    if (mOutputMixObj) (*mOutputMixObj)->Destroy(mOutputMixObj);
    if (mEngineObj) (*mEngineObj)->Destroy(mEngineObj);
    mEngine = nullptr; mOutputMixObj = nullptr; mEngineObj = nullptr;
    mInitialized = false;
}

bool Sound::decodeWAV(const std::string& filename, std::vector<short>& outSamples,
                      SLuint32& outSampleRate, SLuint32& outChannels) {
    AAsset* asset = AAssetManager_open(mAssetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if (!asset) { LOGE("Cannot open %s", filename.c_str()); return false; }
    off_t size = AAsset_getLength(asset);
    if (size < 44) { LOGE("Too small"); AAsset_close(asset); return false; }
    char header[44];
    if (AAsset_read(asset, header, 44) != 44) { LOGE("Read header failed"); AAsset_close(asset); return false; }
    if (memcmp(header+8, "WAVE",4)!=0 || memcmp(header+12,"fmt ",4)!=0) { LOGE("Not WAVE"); AAsset_close(asset); return false; }
    uint16_t fmt = *reinterpret_cast<uint16_t*>(header+20);
    if (fmt != 1) { LOGE("Not PCM"); AAsset_close(asset); return false; }
    outChannels = *reinterpret_cast<uint16_t*>(header+22);
    outSampleRate = *reinterpret_cast<uint32_t*>(header+24);
    uint16_t bits = *reinterpret_cast<uint16_t*>(header+34);
    if (bits != 16) { LOGE("Not 16-bit"); AAsset_close(asset); return false; }
    off_t pos = 44;
    uint32_t dataSize = 0;
    while (pos+8 <= size) {
        AAsset_seek(asset, pos, SEEK_SET);
        char id[5]={0};
        if (AAsset_read(asset, id, 4)!=4) break;
        uint32_t chunkSize;
        if (AAsset_read(asset, &chunkSize, 4)!=4) break;
        if (memcmp(id, "data",4)==0) { dataSize = chunkSize; pos+=8; break; }
        pos += 8 + chunkSize;
    }
    if (dataSize == 0) { LOGE("No data chunk"); AAsset_close(asset); return false; }
    AAsset_seek(asset, pos, SEEK_SET);
    size_t sampleCount = dataSize/2;
    outSamples.resize(sampleCount);
    if (AAsset_read(asset, outSamples.data(), dataSize) != (ssize_t)dataSize) { LOGE("Read data failed"); AAsset_close(asset); return false; }
    AAsset_close(asset);
    return true;
}

bool Sound::loadSound(const std::string& name, const std::string& filename) {
    if (!mInitialized) return false;
    if (mSounds.find(name) != mSounds.end()) return true;
    SoundData data;
    if (!decodeWAV(filename, data.samples, data.sampleRate, data.channels)) return false;
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,1};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, data.channels, data.sampleRate*1000,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        (data.channels==2)?(SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT):SL_SPEAKER_FRONT_CENTER,
        SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource src = {&loc_bufq, &format_pcm};
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
    SLDataSink sink = {&loc_outmix, nullptr};
    const SLInterfaceID ids[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    SLresult result = (*mEngine)->CreateAudioPlayer(mEngine, &data.playerObj, &src, &sink, 2, ids, req);
    if (result != SL_RESULT_SUCCESS) { LOGE("CreateAudioPlayer failed"); return false; }
    result = (*data.playerObj)->Realize(data.playerObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) { (*data.playerObj)->Destroy(data.playerObj); return false; }
    result = (*data.playerObj)->GetInterface(data.playerObj, SL_IID_PLAY, &data.playerPlay);
    if (result != SL_RESULT_SUCCESS) { (*data.playerObj)->Destroy(data.playerObj); return false; }
    result = (*data.playerObj)->GetInterface(data.playerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &data.bufferQueue);
    if (result != SL_RESULT_SUCCESS) { (*data.playerObj)->Destroy(data.playerObj); return false; }
    result = (*data.bufferQueue)->Enqueue(data.bufferQueue, data.samples.data(), data.samples.size()*sizeof(short));
    if (result != SL_RESULT_SUCCESS) { (*data.playerObj)->Destroy(data.playerObj); return false; }
    mSounds[name] = std::move(data);
    LOGI("Loaded sound %s", name.c_str());
    return true;
}

bool Sound::play(const std::string& name) {
    auto it = mSounds.find(name);
    if (it == mSounds.end()) return false;
    SoundData& d = it->second;
    (*d.bufferQueue)->Clear(d.bufferQueue);
    (*d.bufferQueue)->Enqueue(d.bufferQueue, d.samples.data(), d.samples.size()*sizeof(short));
    (*d.playerPlay)->SetPlayState(d.playerPlay, SL_PLAYSTATE_PLAYING);
    return true;
}

// Callback pour la boucle (fonction libre, accède à SoundData publique)
static void musicLoopCallback(SLAndroidSimpleBufferQueueItf bq, void* context) {
    Sound::SoundData* data = (Sound::SoundData*)context;
    if (data) {
        (*bq)->Enqueue(bq, data->samples.data(), data->samples.size() * sizeof(short));
    }
}

bool Sound::playMusic(const std::string& name, bool loop) {
    auto it = mSounds.find(name);
    if (it == mSounds.end()) return false;
    SoundData& d = it->second;
    (*d.playerPlay)->SetPlayState(d.playerPlay, SL_PLAYSTATE_STOPPED);
    (*d.bufferQueue)->Clear(d.bufferQueue);
    (*d.bufferQueue)->Enqueue(d.bufferQueue, d.samples.data(), d.samples.size()*sizeof(short));
    if (loop) {
        (*d.bufferQueue)->RegisterCallback(d.bufferQueue, musicLoopCallback, &d);
    }
    (*d.playerPlay)->SetPlayState(d.playerPlay, SL_PLAYSTATE_PLAYING);
    d.isPlaying = true;
    return true;
}

bool Sound::stopMusic(const std::string& name) {
    auto it = mSounds.find(name);
    if (it == mSounds.end()) return false;
    SoundData& d = it->second;
    (*d.playerPlay)->SetPlayState(d.playerPlay, SL_PLAYSTATE_STOPPED);
    (*d.bufferQueue)->Clear(d.bufferQueue);
    d.isPlaying = false;
    return true;
}

void Sound::unload(const std::string& name) {
    auto it = mSounds.find(name);
    if (it != mSounds.end()) {
        if (it->second.playerObj) (*it->second.playerObj)->Destroy(it->second.playerObj);
        mSounds.erase(it);
    }
}
