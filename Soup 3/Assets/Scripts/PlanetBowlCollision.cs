using UnityEngine;

public class PlanetBowlCollision : MonoBehaviour
{
    [Header("Collision Settings")]
    [SerializeField] private GameObject bowlObject;
    [SerializeField] private float containmentForce = 10f;
    [SerializeField] private float dampingForce = 0.95f;
    [SerializeField] private float bounceForce = 0.7f;
    [SerializeField] private bool enableVisualDebug = true;
    [SerializeField] private bool enableContainmentForces = false;

    [Header("Logging")]
    [Tooltip("If true, logs a message when colliding with the bowl (rate-limited).")]
    [SerializeField] private bool logCollisions = false;
    [SerializeField] private float collisionLogCooldown = 1.0f;

    [Header("Bowl Dimensions")]
    [SerializeField] private float bowlRadius = 2f;
    [SerializeField] private float bowlHeight = 1f;
    [SerializeField] private Vector3 bowlCenter = Vector3.zero;
    
    private Rigidbody planetRigidbody;
    private Collider planetCollider;
    private bool isInsideBowl = false;
    private float _lastCollisionLogTime = -999f;
    
    void Start()
    {
        // Get components
        planetRigidbody = GetComponent<Rigidbody>();
        planetCollider = GetComponent<Collider>();
        
        if (planetRigidbody == null)
        {
            Debug.LogError("PlanetBowlCollision: No Rigidbody found on planet!");
            return;
        }
        
        // Auto-find bowl if not assigned
        if (bowlObject == null)
        {
            bowlObject = GameObject.Find("bowl_03");
            if (bowlObject == null)
            {
                Debug.LogError("PlanetBowlCollision: No bowl object found! Please assign bowl_03 manually.");
                return;
            }
        }
        
        // Configure planet physics
        SetupPlanetPhysics();
        
        // Get bowl dimensions from the bowl object
        UpdateBowlDimensions();
        
        Debug.Log($"Planet collision system initialized. Bowl: {bowlObject.name}");
    }
    
    void SetupPlanetPhysics()
    {
        if (planetRigidbody != null)
        {
            planetRigidbody.useGravity = true;
            planetRigidbody.isKinematic = false;
            planetRigidbody.collisionDetectionMode = CollisionDetectionMode.Continuous;
            planetRigidbody.interpolation = RigidbodyInterpolation.Interpolate;
        }
    }
    
    void UpdateBowlDimensions()
    {
        if (bowlObject != null)
        {
            // Get bowl dimensions from its collider or transform
            Collider bowlCollider = bowlObject.GetComponent<Collider>();
            if (bowlCollider != null)
            {
                bowlCenter = bowlObject.transform.position;
                bowlRadius = Mathf.Max(bowlCollider.bounds.extents.x, bowlCollider.bounds.extents.z);
                bowlHeight = bowlCollider.bounds.extents.y;
            }
            else
            {
                // Fallback to transform-based dimensions
                bowlCenter = bowlObject.transform.position;
                bowlRadius = bowlObject.transform.localScale.x * 0.5f;
                bowlHeight = bowlObject.transform.localScale.y * 0.5f;
            }
        }
    }
    
    void FixedUpdate()
    {
        if (planetRigidbody == null || bowlObject == null) return;
        
        // Update bowl dimensions in case it moves
        UpdateBowlDimensions();
        
        // Optionally apply custom containment forces; otherwise rely solely on physics colliders
        if (enableContainmentForces)
        {
            CheckBowlContainment();
        }
    }
    
    void CheckBowlContainment()
    {
        Vector3 planetPosition = transform.position;
        Vector3 relativePosition = planetPosition - bowlCenter;
        
        // Check horizontal containment (X and Z axes)
        float horizontalDistance = new Vector3(relativePosition.x, 0, relativePosition.z).magnitude;
        float maxHorizontalDistance = bowlRadius - (planetCollider != null ? planetCollider.bounds.extents.x : 0.1f);
        
        // Check vertical containment (Y axis)
        float verticalDistance = Mathf.Abs(relativePosition.y);
        float maxVerticalDistance = bowlHeight - (planetCollider != null ? planetCollider.bounds.extents.y : 0.1f);
        
        bool wasInsideBowl = isInsideBowl;
        isInsideBowl = horizontalDistance <= maxHorizontalDistance && verticalDistance <= maxVerticalDistance;
        
        // Apply containment forces
        if (!isInsideBowl)
        {
            ApplyContainmentForce(relativePosition, horizontalDistance, maxHorizontalDistance, verticalDistance, maxVerticalDistance);
        }
        
        // Apply damping when inside bowl
        if (isInsideBowl)
        {
            planetRigidbody.linearVelocity *= dampingForce;
        }
        
        // Debug info
        if (enableVisualDebug && !isInsideBowl)
        {
            Debug.DrawLine(planetPosition, bowlCenter, Color.red, 0.1f);
        }
    }
    
    void ApplyContainmentForce(Vector3 relativePosition, float horizontalDistance, float maxHorizontalDistance, 
                              float verticalDistance, float maxVerticalDistance)
    {
        Vector3 force = Vector3.zero;
        
        // Horizontal containment (push back towards center)
        if (horizontalDistance > maxHorizontalDistance)
        {
            Vector3 horizontalDirection = new Vector3(relativePosition.x, 0, relativePosition.z).normalized;
            float horizontalForce = (horizontalDistance - maxHorizontalDistance) * containmentForce;
            // Push towards center (opposite of the direction from center to planet)
            force += -horizontalDirection * horizontalForce;
        }
        
        // Vertical containment (push down if too high, push up if too low)
        if (verticalDistance > maxVerticalDistance)
        {
            float verticalDirection = relativePosition.y > 0 ? -1f : 1f;
            float verticalForce = (verticalDistance - maxVerticalDistance) * containmentForce;
            force += Vector3.up * verticalDirection * verticalForce;
        }
        
        // Apply the force
        if (force.magnitude > 0.01f)
        {
            planetRigidbody.AddForce(force, ForceMode.Force);
        }
    }
    
    void OnCollisionEnter(Collision collision)
    {
        // Handle collision with bowl
        if (collision.gameObject == bowlObject)
        {
            // Apply bounce effect
            Vector3 bounceVelocity = Vector3.Reflect(planetRigidbody.linearVelocity, collision.contacts[0].normal);
            planetRigidbody.linearVelocity = bounceVelocity * bounceForce;
            
            if (logCollisions && Time.time - _lastCollisionLogTime > collisionLogCooldown)
            {
                _lastCollisionLogTime = Time.time;
                Debug.Log("Planet collided with bowl!");
            }
        }
    }
    
    void OnDrawGizmosSelected()
    {
        if (!enableVisualDebug || bowlObject == null) return;
        
        // Draw bowl boundary
        Gizmos.color = Color.cyan;
        Gizmos.DrawWireSphere(bowlCenter, bowlRadius);
        
        // Draw vertical bounds
        Gizmos.color = Color.yellow;
        Vector3 topCenter = bowlCenter + Vector3.up * bowlHeight;
        Vector3 bottomCenter = bowlCenter - Vector3.up * bowlHeight;
        Gizmos.DrawWireSphere(topCenter, bowlRadius * 0.1f);
        Gizmos.DrawWireSphere(bottomCenter, bowlRadius * 0.1f);
        
        // Draw planet position
        Gizmos.color = isInsideBowl ? Color.green : Color.red;
        Gizmos.DrawWireSphere(transform.position, 0.2f);
    }
    
    // Public methods for external control
    public void SetContainmentForce(float force)
    {
        containmentForce = Mathf.Max(0f, force);
    }
    
    public void SetBounceForce(float force)
    {
        bounceForce = Mathf.Clamp01(force);
    }
    
    public bool IsInsideBowl()
    {
        return isInsideBowl;
    }
    
    public void ResetPlanetPosition()
    {
        if (bowlObject != null)
        {
            transform.position = bowlCenter + Vector3.up * (bowlHeight * 0.5f);
            if (planetRigidbody != null)
            {
                planetRigidbody.linearVelocity = Vector3.zero;
                planetRigidbody.angularVelocity = Vector3.zero;
            }
        }
    }

    public void SetBowlObject(GameObject targetBowl)
    {
        bowlObject = targetBowl;
        UpdateBowlDimensions();
    }
} 