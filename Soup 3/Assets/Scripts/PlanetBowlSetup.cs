using UnityEngine;

public class PlanetBowlSetup : MonoBehaviour
{
    [Header("Setup Configuration")]
    [SerializeField] private GameObject planetObject;
    [SerializeField] private GameObject bowlObject;
    [SerializeField] private bool autoSetupOnStart = true;
    
    [Header("Planet Physics Settings")]
    [SerializeField] private float planetMass = 1f;
    [SerializeField] private float planetDrag = 0.5f;
    [SerializeField] private float planetAngularDrag = 0.5f;
    
    [Header("Bowl Physics Settings")]
    [SerializeField] private bool makeBowlStatic = true;
    
    void Start()
    {
        if (autoSetupOnStart)
        {
            SetupPlanetAndBowl();
        }
    }
    
    [ContextMenu("Setup Planet and Bowl")]
    public void SetupPlanetAndBowl()
    {
        Debug.Log("=== PLANET & BOWL SETUP ===");
        
        // Auto-find objects if not assigned
        if (planetObject == null)
        {
            planetObject = GameObject.Find("Planet_69");
            if (planetObject == null)
            {
                Debug.LogError("No planet object found! Please assign Planet_69 manually.");
                return;
            }
        }
        
        if (bowlObject == null)
        {
            bowlObject = GameObject.Find("bowl_03");
            if (bowlObject == null)
            {
                Debug.LogError("No bowl object found! Please assign bowl_03 manually.");
                return;
            }
        }
        
        // Setup planet
        SetupPlanet();
        
        // Setup bowl
        SetupBowl();
        
        // Add collision script to planet
        AddCollisionScript();
        
        Debug.Log("✓ Planet and bowl setup completed!");
        Debug.Log("The planet will now collide with the bowl and stay inside it.");
        Debug.Log("Use WASD to tilt the bowl and watch the planet roll around!");
    }
    
    void SetupPlanet()
    {
        Debug.Log($"Setting up planet: {planetObject.name}");
        
        // Ensure planet has Rigidbody
        Rigidbody planetRb = planetObject.GetComponent<Rigidbody>();
        if (planetRb == null)
        {
            planetRb = planetObject.AddComponent<Rigidbody>();
            Debug.Log("✓ Added Rigidbody to planet");
        }
        
        // Configure Rigidbody
        planetRb.mass = planetMass;
        planetRb.linearDamping = planetDrag;
        planetRb.angularDamping = planetAngularDrag;
        planetRb.useGravity = true;
        planetRb.isKinematic = false;
        planetRb.collisionDetectionMode = CollisionDetectionMode.Continuous;
        planetRb.interpolation = RigidbodyInterpolation.Interpolate;
        
        Debug.Log("✓ Configured planet Rigidbody");
        
        // Ensure planet has Collider
        Collider planetCollider = planetObject.GetComponent<Collider>();
        if (planetCollider == null)
        {
            // Add a default sphere collider
            SphereCollider sphereCollider = planetObject.AddComponent<SphereCollider>();
            sphereCollider.radius = 0.5f;
            Debug.Log("✓ Added SphereCollider to planet");
        }
        else
        {
            Debug.Log("✓ Planet already has a collider");
        }
    }
    
    void SetupBowl()
    {
        Debug.Log($"Setting up bowl: {bowlObject.name}");
        
        // Ensure bowl has Rigidbody
        Rigidbody bowlRb = bowlObject.GetComponent<Rigidbody>();
        if (bowlRb == null)
        {
            bowlRb = bowlObject.AddComponent<Rigidbody>();
            Debug.Log("✓ Added Rigidbody to bowl");
        }
        
        // Configure bowl Rigidbody
        if (makeBowlStatic)
        {
            bowlRb.isKinematic = true;
            bowlRb.useGravity = false;
        }
        else
        {
            bowlRb.isKinematic = false;
            bowlRb.useGravity = true;
        }
        
        Debug.Log("✓ Configured bowl Rigidbody");
        
        // Ensure bowl has Collider
        Collider bowlCollider = bowlObject.GetComponent<Collider>();
        if (bowlCollider == null)
        {
            // Try to add a mesh collider based on the mesh
            MeshFilter meshFilter = bowlObject.GetComponent<MeshFilter>();
            if (meshFilter != null && meshFilter.sharedMesh != null)
            {
                MeshCollider meshCollider = bowlObject.AddComponent<MeshCollider>();
                meshCollider.convex = true;
                Debug.Log("✓ Added MeshCollider to bowl");
            }
            else
            {
                // Fallback to sphere collider
                SphereCollider sphereCollider = bowlObject.AddComponent<SphereCollider>();
                sphereCollider.radius = 2f;
                Debug.Log("✓ Added SphereCollider to bowl (fallback)");
            }
        }
        else
        {
            Debug.Log("✓ Bowl already has a collider");
        }
        
        // Ensure bowl has BowlTiltController
        BowlTiltController tiltController = bowlObject.GetComponent<BowlTiltController>();
        if (tiltController == null)
        {
            tiltController = bowlObject.AddComponent<BowlTiltController>();
            Debug.Log("✓ Added BowlTiltController to bowl");
        }
        else
        {
            Debug.Log("✓ Bowl already has BowlTiltController");
        }
    }
    
    void AddCollisionScript()
    {
        // Remove existing collision script if present
        PlanetBowlCollision existingCollision = planetObject.GetComponent<PlanetBowlCollision>();
        if (existingCollision != null)
        {
            DestroyImmediate(existingCollision);
        }
        
        // Add new collision script
        PlanetBowlCollision collisionScript = planetObject.AddComponent<PlanetBowlCollision>();
        
        // Configure the script
        var serializedObject = new UnityEditor.SerializedObject(collisionScript);
        serializedObject.FindProperty("bowlObject").objectReferenceValue = bowlObject;
        serializedObject.ApplyModifiedProperties();
        
        Debug.Log("✓ Added PlanetBowlCollision script to planet");
    }
    
    [ContextMenu("Reset Planet Position")]
    public void ResetPlanetPosition()
    {
        if (planetObject != null && bowlObject != null)
        {
            // Position planet inside the bowl
            Vector3 bowlPosition = bowlObject.transform.position;
            Vector3 planetPosition = bowlPosition + Vector3.up * 2f; // Above the bowl
            
            planetObject.transform.position = planetPosition;
            
            // Reset velocity
            Rigidbody planetRb = planetObject.GetComponent<Rigidbody>();
            if (planetRb != null)
            {
                planetRb.linearVelocity = Vector3.zero;
                planetRb.angularVelocity = Vector3.zero;
            }
            
            Debug.Log("✓ Reset planet position");
        }
    }
    
    [ContextMenu("Test Collision")]
    public void TestCollision()
    {
        if (planetObject != null && bowlObject != null)
        {
            // Move planet to test collision
            Vector3 bowlPosition = bowlObject.transform.position;
            Vector3 testPosition = bowlPosition + Vector3.up * 5f; // High above bowl
            
            planetObject.transform.position = testPosition;
            
            Rigidbody planetRb = planetObject.GetComponent<Rigidbody>();
            if (planetRb != null)
            {
                planetRb.linearVelocity = Vector3.zero;
                planetRb.angularVelocity = Vector3.zero;
            }
            
            Debug.Log("✓ Moved planet to test position - it should fall and collide with the bowl");
        }
    }
} 