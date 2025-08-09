// AudioPluginRNBO.cpp
// Unity native audio plugin that wraps RNBO exported patch from C++export/Space Echo.cpp
// Build as a bundle/dll and place into Assets/Plugins to have Unity load the effect named "RNBOEffect".

#include "AudioPluginInterface.h"

// Include the RNBO patch header
#include "../C++export/Space Echo.cpp" // make sure build system includes cpp; or split into headers

using namespace RNBO;

static rnbomatic g_patch; // global patch
static double g_sampleRate = 48000.0;

static void PreparePatch(double sr, int maxBlock)
{
    g_patch.prepareToProcess(sr, maxBlock);
}

static void ProcessPatch(float** inputs, float** outputs, int channels, int numFrames)
{
    g_patch.process(inputs, outputs, numFrames, channels);
}

// -------------------------------------------------------------
// Unity effect wrapper
// -------------------------------------------------------------
DECLARE_EFFECT(RNBOEffect)

static void RNBOEffect_CreateCallback(AudioEffectState* state)
{
    g_sampleRate = state->samplerate;
    PreparePatch(g_sampleRate, 1024);
}

static void RNBOEffect_ReleaseCallback(AudioEffectState* state) {}

static void RNBOEffect_ProcessCallback(AudioEffectState* state,
                                       float* inBuffer, float* outBuffer,
                                       int length, int inChannels, int outChannels)
{
    // Setup interleaved buffers to separate channel pointers
    static float* inPtrs[2];
    static float* outPtrs[2];
    inPtrs[0] = inBuffer;
    inPtrs[1] = (inChannels > 1) ? inBuffer + length : inBuffer;
    outPtrs[0] = outBuffer;
    outPtrs[1] = (outChannels > 1) ? outBuffer + length : outBuffer;

    ProcessPatch(inPtrs, outPtrs, 2, length);
}

static AudioEffectDefinition definition = {
    sizeof(AudioEffectDefinition),
    RNBOEffect_CreateCallback,
    RNBOEffect_ReleaseCallback,
    RNBOEffect_ProcessCallback,
    nullptr, // no sidechain
    nullptr, // no parameter change
    0, 0, nullptr
};

#define DLL_PUBLIC extern "C" __attribute__((visibility("default")))

DLL_PUBLIC AudioEffectDefinition* UnityGetAudioEffectDefinitions(unsigned int* count)
{
    *count = 1;
    return &definition;
}

END_DECLARE_EFFECT
