using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public class PlanetAudio : MonoBehaviour
{
    [Tooltip("Sound played when this planet collides with another planet.")]
    public AudioClip collisionClip;

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
