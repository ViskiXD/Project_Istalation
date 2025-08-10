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
        if (instance != null && instance != this)
        {
            Destroy(gameObject);
            return;
        }
        instance = this;
        DontDestroyOnLoad(gameObject);

        audioSource = GetComponent<AudioSource>();
        var second = gameObject.AddComponent<AudioSource>();

        // Common config helper
        void ConfigSource(AudioSource s)
        {
            s.playOnAwake = false;
            s.loop = true;
            s.spatialBlend = 0f;
            s.rolloffMode = AudioRolloffMode.Logarithmic;
            s.volume = 0f; // start silent for fade
        }
        ConfigSource(audioSource);
        ConfigSource(second);

        sources = new[] { audioSource, second };

        // Start first track
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
            // do nothing; sources persist
        };
    }

    void Update()
    {
        if (musicTracks != null && musicTracks.Count > 0 && Input.GetKeyDown(KeyCode.Q))
        {
            PlayNextTrack();
        }
    }

    // Public wrapper for external callers (e.g., controller input)
    public void NextTrack()
    {
        PlayNextTrack(false);
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
        newSrc.volume = 0f;
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
