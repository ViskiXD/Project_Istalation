using UnityEngine;
using UnityEngine.Audio;

// Attach this to any GameObject. Assign the AudioMixerGroup that has the Space Echo effect.
// It reads the bowl tilt (from BowlTiltController) and maps tilt to Space Echo parameters in real-time.
public class SpaceEchoTiltController : MonoBehaviour
{
    [Header("Routing")]
    public AudioMixerGroup targetGroup; // used to find its AudioMixer for fallback control
    public AudioMixer explicitMixer;    // optional explicit mixer reference (if group not set)
    public BowlTiltController bowlTilt;

    [Header("Parameter Mapping (normalized 0..1)")]
    [Range(0f, 1f)] public float echoRateAtMaxTilt = 0.8f;
    [Range(0f, 1f)] public float feedbackAtMaxTilt = 0.7f;
    [Range(0f, 1f)] public float reverbAtMaxTilt = 0.5f;

    private SpaceEchoHandle handle;
    private AudioMixer mixer;
    private int? echoRateIndex;
    private int? feedbackIndex;
    private int? reverbIndex;
    private bool warned;

    void Start()
    {
        if (bowlTilt == null)
            bowlTilt = FindFirstObjectByType<BowlTiltController>();

        handle = new SpaceEchoHandle();

        // Cache mixer for fallback path
        mixer = explicitMixer != null ? explicitMixer : (targetGroup != null ? targetGroup.audioMixer : null);

        echoRateIndex = SpaceEchoHandle.GetFirstParamIndexByName("Echo_Rate");
        feedbackIndex = SpaceEchoHandle.GetFirstParamIndexByName("FeedBack");
        if (!feedbackIndex.HasValue)
            feedbackIndex = SpaceEchoHandle.GetFirstParamIndexByName("FeedBack_1");
        reverbIndex = SpaceEchoHandle.GetFirstParamIndexByName("Reverb_Gain");
    }

    void Update()
    {
        if (bowlTilt == null) return;

        Vector3 tilt = bowlTilt.CurrentTilt;
        float maxAngle = Mathf.Max(1f, bowlTilt.MaxTiltAngle);
        float amount = Mathf.Clamp01(new Vector2(tilt.x, tilt.z).magnitude / maxAngle);

        bool setByHandle = false;
        if (handle != null)
        {
            bool any = false;
            if (echoRateIndex.HasValue)
            {
                handle.SetParamValueNormalized(echoRateIndex.Value, amount * echoRateAtMaxTilt);
                any = true;
            }
            if (feedbackIndex.HasValue)
            {
                handle.SetParamValueNormalized(feedbackIndex.Value, amount * feedbackAtMaxTilt);
                any = true;
            }
            if (reverbIndex.HasValue)
            {
                handle.SetParamValueNormalized(reverbIndex.Value, amount * reverbAtMaxTilt);
                any = true;
            }
            setByHandle = any;
            handle.Update();
        }

        // Fallback: drive exposed mixer parameters directly (requires you to expose these in the AudioMixer UI)
        if (!setByHandle && mixer != null)
        {
            bool anyMixer = false;
            anyMixer |= mixer.SetFloat("Echo_Rate", amount * echoRateAtMaxTilt);
            anyMixer |= mixer.SetFloat("FeedBack", amount * feedbackAtMaxTilt) || mixer.SetFloat("FeedBack_1", amount * feedbackAtMaxTilt);
            anyMixer |= mixer.SetFloat("Reverb_Gain", amount * reverbAtMaxTilt);
            if (!anyMixer && !warned)
            {
                warned = true;
                Debug.LogWarning("SpaceEchoTiltController: Neither RNBO handle nor exposed mixer parameters could be set. Ensure the effect is on a Mixer Group and its parameters are exposed with names Echo_Rate, FeedBack (or FeedBack_1), Reverb_Gain.");
            }
        }
    }
}
