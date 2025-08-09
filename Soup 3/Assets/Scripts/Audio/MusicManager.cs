using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections.Generic;

/// <summary>
/// Plays a looping ambient music track across the whole experience.
/// The manager persists across scene loads.
/// </summary>
[AddComponentMenu("Audio/Music Manager")]
[RequireComponent(typeof(AudioSource))]
public class MusicManager : MonoBehaviour
{
    [Tooltip("Ambient background music clip.")]
    public AudioClip ambientClip;

    [Tooltip("Master volume for the ambient music.")]
    [Range(0f, 1f)]
    public float volume = 0.6f;

    [Header("Playlist")] 
    [Tooltip("Ordered list of background music tracks. If empty, falls back to 'ambientClip'.")] 
    public List<AudioClip> musicTracks = new List<AudioClip>();
    [Tooltip("Volume for each track (0-1). If index is out of range, 1 is used.")]
    public List<float> trackVolumes = new List<float>();

    [Tooltip("Duration (seconds) for cross-fades between music tracks.")]
    public float crossfadeDuration = 2f;

    private AudioSource[] sources;
    private int activeSourceIndex = 0;

    private int currentTrackIndex = -1;

    private static MusicManager instance;
    private AudioSource audioSource;

    void Awake()
    {
        if (instance != null)
        {
            Destroy(gameObject);
            return;
        }
        instance = this;
        DontDestroyOnLoad(gameObject);

        audioSource = GetComponent<AudioSource>();
        // create secondary source for crossfading
        var second = gameObject.AddComponent<AudioSource>();

        // Common config helper
        void ConfigSource(AudioSource s)
        {
            s.playOnAwake = false;
            s.loop = true;
            s.spatialBlend = 0f;
            s.rolloffMode = AudioRolloffMode.Logarithmic;
        }
        ConfigSource(audioSource);
        ConfigSource(second);

        sources = new[] { audioSource, second };

        // Initial volumes zero so first fade-in uses correct per-track level
        audioSource.volume = 0f;
        second.volume = 0f;

        float initialTargetVol = 1f;
        if (trackVolumes != null && trackVolumes.Count > 0) initialTargetVol = Mathf.Clamp01(trackVolumes.Count>0?trackVolumes[0]:1f);
        currentTrackIndex = -1;
        if (musicTracks != null && musicTracks.Count > 0)
        {
            PlayNextTrack(fadeIn:true);
        }
        else if (ambientClip != null)
        {
            StartCoroutine(CrossfadeToClip(ambientClip, volume, true));
        }

        // Ensure music keeps playing across scene reloads
        SceneManager.sceneLoaded += (_, __) => {
            if (!audioSource.isPlaying)
            {
                if (musicTracks != null && musicTracks.Count > 0)
                {
                    PlayCurrentTrack();
                }
                else if (ambientClip != null)
                {
                    audioSource.clip = ambientClip;
                    audioSource.Play();
                }
            }
        };
    }

    void Update()
    {
        if (musicTracks != null && musicTracks.Count > 0 && Input.GetKeyDown(KeyCode.Q))
        {
            PlayNextTrack();
        }
    }

    void PlayNextTrack(bool fadeIn = false)
    {
        if (musicTracks == null || musicTracks.Count == 0) return;
        currentTrackIndex = (currentTrackIndex + 1) % musicTracks.Count;
        PlayCurrentTrack(fadeIn);
    }

    void PlayCurrentTrack(bool fadeIn = false)
    {
        if (musicTracks == null || musicTracks.Count == 0) return;
        var clip = musicTracks[currentTrackIndex];
        if (clip == null) return;
        float vol = volume; // master volume
        if (trackVolumes != null && currentTrackIndex < trackVolumes.Count)
        {
            vol *= Mathf.Clamp01(trackVolumes[currentTrackIndex]);
        }
        StartCoroutine(CrossfadeToClip(clip, vol, fadeIn));
    }

    System.Collections.IEnumerator CrossfadeToClip(AudioClip clip, float targetVol, bool fadeInOnly)
    {
        // Determine sources
        int newSourceIndex = 1 - activeSourceIndex;
        AudioSource newSrc = sources[newSourceIndex];
        AudioSource oldSrc = sources[activeSourceIndex];

        newSrc.clip = clip;
        newSrc.volume = fadeInOnly ? 0f : 0f;
        newSrc.Play();

        float t = 0f;
        while (t < crossfadeDuration)
        {
            t += Time.unscaledDeltaTime;
            float lerp = Mathf.Clamp01(t / crossfadeDuration);
            newSrc.volume = Mathf.Lerp(0f, targetVol, lerp);
            if (!fadeInOnly)
            {
                oldSrc.volume = Mathf.Lerp(oldSrc.volume, 0f, lerp);
            }
            yield return null;
        }

        newSrc.volume = targetVol;
        if (!fadeInOnly)
        {
            oldSrc.Stop();
        }
        activeSourceIndex = newSourceIndex;
    }
}
