using UnityEngine;

#if ENABLE_INPUT_SYSTEM && !UNITY_INPUT_SYSTEM_DISABLE
using UnityEngine.InputSystem;
#endif

/// <summary>
/// Bridges Sony DualSense inputs to the existing keyboard-driven controls.
/// - Gyro/accelerometer controls bowl tilt (in parallel with keyboard)
/// - R2 spawns a planet
/// - L2 removes one planet
/// - R1 switches to the next background music track
/// </summary>
public class DualSenseControllerInput : MonoBehaviour
{
    [Header("References")]
    public BowlTiltController bowl;
    public PlanetSpawner spawner;
    public MusicManager music;

    [Header("Tilt Mapping")] 
    [Range(0.1f, 30f)] public float tiltSmoothing = 8f;
    [Tooltip("Map right stick to tilt when gyro sensors are unavailable (Unity currently does not expose DualSense gyro on desktop).")]
    public bool useRightStickFallback = true;

    // Internal state
    private Vector3 targetTiltEuler;
    private bool haveReading;

    // Button edge detection
    private bool r2WasPressed;
    private bool l2WasPressed;
    private bool r1WasPressed;

    void Awake()
    {
        if (bowl == null) bowl = FindFirstObjectByType<BowlTiltController>();
        if (spawner == null) spawner = FindFirstObjectByType<PlanetSpawner>();
        if (music == null) music = FindFirstObjectByType<MusicManager>();
    }

    void Update()
    {
#if ENABLE_INPUT_SYSTEM && !UNITY_INPUT_SYSTEM_DISABLE
        // Prefer generic Gamepad — Unity does not expose DualSense gyro on desktop platforms.
        Gamepad gp = Gamepad.current;

        // If we have a gamepad, optionally map right stick to tilt as a fallback.
        haveReading = false;
        if (gp != null && useRightStickFallback)
        {
            Vector2 stick = gp.rightStick.ReadValue();
            if (stick.sqrMagnitude > 0.0001f)
            {
                // Map stick to bowl pitch/roll (x=forward/back, y=left/right)
                // Y up on stick should tilt bowl forward (increase x tilt)
                float max = bowl != null ? bowl.MaxTiltAngle : 25f;
                targetTiltEuler = new Vector3(stick.y * max, 0f, -stick.x * max);
                haveReading = true;
            }
        }

        // Clamp and smooth to bowl constraints
        if (haveReading && bowl != null)
        {
            float max = bowl.MaxTiltAngle;
            targetTiltEuler.x = Mathf.Clamp(Normalize180(targetTiltEuler.x), -max, max);
            targetTiltEuler.z = Mathf.Clamp(Normalize180(targetTiltEuler.z), -max, max);

            // Smooth and push to bowl as external tilt override
            Vector3 smoothed = Vector3.Lerp(bowl.CurrentTilt, targetTiltEuler, 1f - Mathf.Exp(-tiltSmoothing * Time.deltaTime));
            bowl.SetExternalTilt(smoothed);
        }

        // Buttons: R2 spawn, L2 remove, R1 next track
        if (gp != null)
        {
            bool r2 = gp.rightTrigger.ReadValue() > 0.5f;
            bool l2 = gp.leftTrigger.ReadValue() > 0.5f;
            bool r1 = gp.rightShoulder.isPressed;

            if (r2 && !r2WasPressed && spawner != null) spawner.SendMessage("SpawnRandomPlanet", SendMessageOptions.DontRequireReceiver);
            if (l2 && !l2WasPressed && spawner != null) spawner.SendMessage("RemoveOnePlanet", SendMessageOptions.DontRequireReceiver);
            if (r1 && !r1WasPressed && music != null) music.SendMessage("NextTrack", SendMessageOptions.DontRequireReceiver);

            r2WasPressed = r2;
            l2WasPressed = l2;
            r1WasPressed = r1;
        }
#endif
    }

    static float Normalize180(float angle)
    {
        angle = Mathf.Repeat(angle + 180f, 360f) - 180f;
        return angle;
    }
}


