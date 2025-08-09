using UnityEngine;
using UnityEngine.Audio;

// Attach this to any GameObject. Assign the AudioMixerGroup that has the Space Echo effect.
// It reads the bowl tilt (from BowlTiltController) and maps tilt to Space Echo parameters in real-time.
public class SpaceEchoTiltController : MonoBehaviour
{
    [Header("Routing")]
    public AudioMixerGroup targetGroup; // not used directly but kept for clarity
    public BowlTiltController bowlTilt;

    [Header("Parameter Mapping (normalized 0..1)")]
    [Range(0f, 1f)] public float echoRateAtMaxTilt = 0.8f;
    [Range(0f, 1f)] public float feedbackAtMaxTilt = 0.7f;
    [Range(0f, 1f)] public float reverbAtMaxTilt = 0.5f;

    private SpaceEchoHandle handle;
    private int? echoRateIndex;
    private int? feedbackIndex;
    private int? reverbIndex;
    private bool warned;

    void Start()
    {
        if (bowlTilt == null)
            bowlTilt = FindFirstObjectByType<BowlTiltController>();

        handle = new SpaceEchoHandle();

        echoRateIndex = SpaceEchoHandle.GetFirstParamIndexByName("Echo_Rate");
        feedbackIndex = SpaceEchoHandle.GetFirstParamIndexByName("FeedBack");
        if (!feedbackIndex.HasValue)
            feedbackIndex = SpaceEchoHandle.GetFirstParamIndexByName("FeedBack_1");
        reverbIndex = SpaceEchoHandle.GetFirstParamIndexByName("Reverb_Gain");
    }

    void Update()
    {
        if (handle == null || bowlTilt == null) return;

        Vector3 tilt = bowlTilt.CurrentTilt;
        float maxAngle = Mathf.Max(1f, bowlTilt.MaxTiltAngle);
        float amount = Mathf.Clamp01(new Vector2(tilt.x, tilt.z).magnitude / maxAngle);

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

        if (!any && !warned)
        {
            warned = true;
            Debug.LogWarning("SpaceEchoTiltController: Could not find parameter indices. Check parameter names in the Mixer UI and tell me exact labels.");
        }

        handle.Update();
    }
}
