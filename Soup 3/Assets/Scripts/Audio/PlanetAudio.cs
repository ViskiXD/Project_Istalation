using UnityEngine;
using UnityEngine.Audio;

[RequireComponent(typeof(AudioSource))]
public class PlanetAudio : MonoBehaviour
{
    [Tooltip("Sound played when this planet collides with another planet.")]
    public AudioClip collisionClip;

    [Tooltip("Mixer group to route this planet's audio through (e.g., 'Planets' group with RNBO effect). Optional.")]
    public AudioMixerGroup outputGroup;

    [Tooltip("Volume multiplier for the collision sound.")]
    [Range(0f, 1f)]
    public float volume = 1f;

    [Tooltip("Minimum collision impulse (relative velocity) required to trigger the sound.")]
    public float minCollisionSpeed = 0.5f;

    [Tooltip("Cooldown time between collision sounds (seconds). Prevents rapid retriggering on sliding collisions.")]
    public float cooldown = 0.15f;

    private AudioSource audioSource;
    private float lastPlayTime = -Mathf.Infinity;

    void Awake()
    {
        audioSource = GetComponent<AudioSource>();
        audioSource.playOnAwake = false;
        audioSource.spatialBlend = 1f;   // 3D sound
        audioSource.rolloffMode = AudioRolloffMode.Logarithmic;
        if (collisionClip != null)
        {
            audioSource.clip = collisionClip; // For editor preview convenience
        }

        // Ensure RNBO processing is applied on this AudioSource via RNBOAudioFilter
        if (GetComponent<RNBOAudioFilter>() == null)
        {
            gameObject.AddComponent<RNBOAudioFilter>();
        }

        // Auto-route to a mixer group if none assigned so effects like Space Echo can process the sound
        if (outputGroup == null)
        {
            TryAutoAssignMixerGroup();
        }

        if (outputGroup != null)
        {
            audioSource.outputAudioMixerGroup = outputGroup;
        }
    }

    void TryAutoAssignMixerGroup()
    {
        // Prefer a group named "Planets"; otherwise fall back to any "Master" group
        var mixers = Resources.FindObjectsOfTypeAll<AudioMixer>();
        foreach (var mixer in mixers)
        {
            var groups = mixer.FindMatchingGroups("Planets");
            if (groups != null && groups.Length > 0)
            {
                outputGroup = groups[0];
                return;
            }
        }
        foreach (var mixer in mixers)
        {
            var groups = mixer.FindMatchingGroups("Master");
            if (groups != null && groups.Length > 0)
            {
                outputGroup = groups[0];
                return;
            }
        }
    }

    void OnCollisionEnter(Collision collision)
    {
        // Only react if we hit another planet that also has PlanetAudio
        if (collision.gameObject.GetComponent<PlanetAudio>() == null)
            return;

        if (Time.time - lastPlayTime < cooldown)
            return;

        float speed = collision.relativeVelocity.magnitude;
        if (speed < minCollisionSpeed)
            return;

        if (collisionClip != null)
        {
            audioSource.PlayOneShot(collisionClip, volume);
            lastPlayTime = Time.time;
        }
    }
}
