# Building `AudioPluginRNBO` for Unity

This plug-in wraps the RNBO export (`C++export/Space Echo.cpp`) so it can run as a native Unity audio effect.

## 1. Prerequisites
* Xcode (macOS) or Visual Studio (Windows)
* Unity installation (for the AudioPluginInterface headers)

## 2. Project layout
```
Plugins/
  AudioPluginRNBO.cpp       -> **wrapper** (this repo)
C++export/
  Space Echo.cpp            -> **RNBO patch**
```

## 3. Create Xcode / VS project
1. Copy Unity’s *AudioPluginDemo* template (found in `…/Unity/Editor/Data/PluginAPI/`)
2. Replace the demo `AudioPluginDemo.cpp` with `AudioPluginRNBO.cpp`
3. Add `C++export/Space Echo.cpp` to the project

## 4. Build
* macOS: build target **AudioPluginRNBO.bundle** (Release, x86_64 + arm64)
* Windows: build **AudioPluginRNBO.dll** (Release, x86_64)

## 5. Install
Place the build-product in your Unity project:
```
Assets/Plugins/AudioPluginRNBO.bundle   # macOS
Assets/Plugins/x86_64/AudioPluginRNBO.dll # Windows
```
Unity log should show:
```
Loaded audio plug-in 'AudioPluginRNBO' (1 effect)
```

## 6. Use in Unity
1. Open *Audio Mixer* window -> create **Planets** group.
2. Add **RNBOEffect** to the group (appears under *Native* effects).
3. In *PlanetAudio.cs* set `outputAudioMixerGroup` of each planet’s `AudioSource` to **Planets**.

Now every collision sound passes through your RNBO patch with 2-second cross-fades still intact.
