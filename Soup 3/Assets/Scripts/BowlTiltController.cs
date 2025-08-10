using UnityEngine;

public class BowlTiltController : MonoBehaviour
{
    [Header("Bowl Tilt Settings")]
    [SerializeField] private float maxTiltAngle = 25f;
    [SerializeField] private float tiltSpeed = 70f; // Degrees per second

    private Vector3 currentRotation;
    private Vector3 externalTiltEuler;
    private bool hasExternalTilt;

    public float MaxTiltAngle => maxTiltAngle;

    void Start()
    {
        currentRotation = Vector3.zero;
        
        // Simple physics setup - don't mess with colliders
        Rigidbody rb = GetComponent<Rigidbody>();
        if (rb != null)
        {
            // If a Rigidbody is present, ensure we don't accidentally make the bowl unaffected by gravity unless desired
            rb.useGravity = false;
            rb.isKinematic = true;
        }
    }

    void Update()
    {
        if (!hasExternalTilt)
        {
            HandleInput();
        }
        ApplyRotation();
        hasExternalTilt = false; // external must be refreshed each frame
    }

    void HandleInput()
    {
        float forwardInput = 0f;
        float sideInput = 0f;

        // W/S for forward/backward tilt
        if (Input.GetKey(KeyCode.W))
            forwardInput = 1f;
        else if (Input.GetKey(KeyCode.S))
            forwardInput = -1f;

        // A/D for left/right tilt  
        if (Input.GetKey(KeyCode.A))
            sideInput = 1f;
        else if (Input.GetKey(KeyCode.D))
            sideInput = -1f;

        // Apply rotation
        if (forwardInput != 0f || sideInput != 0f)
        {
            float rotationDelta = tiltSpeed * Time.deltaTime;
            
            currentRotation.x += forwardInput * rotationDelta;
            currentRotation.x = Mathf.Clamp(currentRotation.x, -maxTiltAngle, maxTiltAngle);

            currentRotation.z += sideInput * rotationDelta;
            currentRotation.z = Mathf.Clamp(currentRotation.z, -maxTiltAngle, maxTiltAngle);
        }
    }

    void ApplyRotation()
    {
        Vector3 euler = hasExternalTilt ? externalTiltEuler : currentRotation;
        transform.rotation = Quaternion.Euler(euler.x, 0f, euler.z);
    }

    [ContextMenu("Reset Bowl")]
    public void ResetBowl()
    {
        currentRotation = Vector3.zero;
        externalTiltEuler = Vector3.zero;
        hasExternalTilt = false;
        
        // Also reset soup simulation if present
        SoupFluidController soupController = Object.FindFirstObjectByType<SoupFluidController>();
        if (soupController != null)
        {
            soupController.ResetSimulation();
        }
    }
    
    // Public property for soup controller integration
    public Vector3 CurrentTilt => currentRotation;

    /// <summary>
    /// Allows external systems (e.g., DualSense gyroscope) to set the bowl tilt this frame.
    /// Keyboard input will be ignored for this frame while this is active.
    /// </summary>
    public void SetExternalTilt(Vector3 euler)
    {
        euler.x = Mathf.Clamp(euler.x, -maxTiltAngle, maxTiltAngle);
        euler.z = Mathf.Clamp(euler.z, -maxTiltAngle, maxTiltAngle);
        externalTiltEuler = euler;
        currentRotation = euler; // keep state in sync for readers
        hasExternalTilt = true;
    }
}