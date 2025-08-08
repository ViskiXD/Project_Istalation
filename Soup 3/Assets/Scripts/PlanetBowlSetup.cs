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
    [SerializeField] private bool forceProjectGravity = true;
    
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
        
        // Ensure project gravity is sane
        if (forceProjectGravity && Mathf.Approximately(Physics.gravity.sqrMagnitude, 0f))
        {
            Physics.gravity = new Vector3(0, -9.81f, 0);
            Debug.Log("✓ Physics.gravity set to (0,-9.81,0)");
        }

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
        // Ensure collider is not a trigger
        planetCollider = planetObject.GetComponent<Collider>();
        if (planetCollider != null)
        {
            planetCollider.isTrigger = false;
        }
    }
    
    void SetupBowl()
    {
        Debug.Log($"Setting up bowl: {bowlObject.name}");
        
        // Rigidbody/Collider configuration depends on static vs dynamic bowl
        Rigidbody bowlRb = bowlObject.GetComponent<Rigidbody>();

        if (makeBowlStatic)
        {
            // For a static bowl that can be tilted manually, prefer NO Rigidbody so we can use a non-convex MeshCollider
            if (bowlRb != null)
            {
                if (Application.isPlaying)
                    Destroy(bowlRb);
                else
                    DestroyImmediate(bowlRb);
                bowlRb = null;
                Debug.Log("✓ Removed Rigidbody from bowl for static setup");
            }

            // Ensure a non-convex mesh collider exists (prefer adding to the child that actually has the MeshFilter)
            MeshCollider meshCollider = bowlObject.GetComponent<MeshCollider>();
            if (meshCollider == null)
            {
                GameObject targetForCollider = null;
                MeshFilter meshFilter = bowlObject.GetComponent<MeshFilter>();
                if (meshFilter != null && meshFilter.sharedMesh != null)
                {
                    targetForCollider = bowlObject;
                }
                else
                {
                    // Search children for the visual mesh
                    var childMeshFilters = bowlObject.GetComponentsInChildren<MeshFilter>();
                    foreach (var mf in childMeshFilters)
                    {
                        if (mf != null && mf.sharedMesh != null)
                        {
                            targetForCollider = mf.gameObject;
                            break;
                        }
                    }
                }

                if (targetForCollider != null)
                {
                    MeshCollider childMeshCollider = targetForCollider.GetComponent<MeshCollider>();
                    if (childMeshCollider == null)
                    {
                        childMeshCollider = targetForCollider.AddComponent<MeshCollider>();
                    }
                    childMeshCollider.convex = false;
                    childMeshCollider.isTrigger = false;
                    Debug.Log($"✓ Added/Configured MeshCollider on '{targetForCollider.name}' (non-convex)");
                }
                else
                {
                    // Fallback to sphere collider on root if no mesh found
                    SphereCollider sphereCollider = bowlObject.AddComponent<SphereCollider>();
                    sphereCollider.radius = 2f;
                    sphereCollider.isTrigger = false;
                    Debug.Log("✓ Added SphereCollider to bowl (fallback)");
                }
            }
            else
            {
                meshCollider.convex = false;
                meshCollider.isTrigger = false;
                Debug.Log("✓ Configured existing MeshCollider as non-convex");
            }
            // Ensure collider is not a trigger
            Collider bowlColliderNonRigid = bowlObject.GetComponent<Collider>();
            if (bowlColliderNonRigid != null)
            {
                bowlColliderNonRigid.isTrigger = false;
            }
        }
        else
        {
            // Dynamic bowl: keep/add Rigidbody (but note: MeshCollider must be convex with Rigidbody present)
            if (bowlRb == null)
            {
                bowlRb = bowlObject.AddComponent<Rigidbody>();
                Debug.Log("✓ Added Rigidbody to bowl");
            }
            bowlRb.isKinematic = false;
            bowlRb.useGravity = true;

            // Ensure collider is present and convex
            MeshCollider meshCollider = bowlObject.GetComponent<MeshCollider>();
            if (meshCollider == null)
            {
                GameObject targetForCollider = null;
                MeshFilter meshFilter = bowlObject.GetComponent<MeshFilter>();
                if (meshFilter != null && meshFilter.sharedMesh != null)
                {
                    targetForCollider = bowlObject;
                }
                else
                {
                    var childMeshFilters = bowlObject.GetComponentsInChildren<MeshFilter>();
                    foreach (var mf in childMeshFilters)
                    {
                        if (mf != null && mf.sharedMesh != null)
                        {
                            targetForCollider = mf.gameObject;
                            break;
                        }
                    }
                }

                if (targetForCollider != null)
                {
                    MeshCollider childMeshCollider = targetForCollider.GetComponent<MeshCollider>();
                    if (childMeshCollider == null)
                    {
                        childMeshCollider = targetForCollider.AddComponent<MeshCollider>();
                    }
                    childMeshCollider.convex = true; // Required when Rigidbody is present
                    childMeshCollider.isTrigger = false;
                    Debug.Log($"✓ Added/Configured MeshCollider on '{targetForCollider.name}' (convex for Rigidbody)");
                }
                else
                {
                    SphereCollider sphereCollider = bowlObject.GetComponent<SphereCollider>();
                    if (sphereCollider == null)
                    {
                        sphereCollider = bowlObject.AddComponent<SphereCollider>();
                        sphereCollider.radius = 2f;
                        sphereCollider.isTrigger = false;
                        Debug.Log("✓ Added SphereCollider to bowl (fallback)");
                    }
                }
            }
            else
            {
                meshCollider.convex = true; // Required when Rigidbody is present
                meshCollider.isTrigger = false;
                Debug.Log("✓ Configured existing MeshCollider as convex for Rigidbody");
            }
            // Ensure collider is not a trigger
            Collider bowlColliderRigid = bowlObject.GetComponent<Collider>();
            if (bowlColliderRigid != null)
            {
                bowlColliderRigid.isTrigger = false;
            }
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
            if (Application.isPlaying)
                Destroy(existingCollision);
            else
                DestroyImmediate(existingCollision);
        }

        // Add new collision script
        PlanetBowlCollision collisionScript = planetObject.AddComponent<PlanetBowlCollision>();
        // Configure the script without relying on UnityEditor APIs
        collisionScript.SetBowlObject(bowlObject);

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