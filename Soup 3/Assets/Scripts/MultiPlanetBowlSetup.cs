using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class MultiPlanetBowlSetup : MonoBehaviour
{
    [Header("References")]
    [Tooltip("Leave empty to auto-find all GameObjects whose name starts with 'Planet_'")]
    [SerializeField] private List<GameObject> planetObjects = new List<GameObject>();
    [SerializeField] private GameObject bowlObject;

    [Header("Search Settings")]
    [SerializeField] private bool autoFindPlanetsByName = true;
    [SerializeField] private string planetNamePrefix = "Planet_";

    [Header("Planet Physics Settings")]
    [SerializeField] private float planetMass = 1f;
    [SerializeField] private float planetDrag = 0.5f;       // Linear damping
    [SerializeField] private float planetAngularDrag = 0.5f; // Angular damping

    [Header("Bowl Physics Settings")]
    [SerializeField] private bool makeBowlStatic = true;      // Static bowl uses non-convex MeshCollider
    [SerializeField] private bool forceProjectGravity = true; // Ensure Physics.gravity is valid

    void Start()
    {
        SetupPlanetsAndBowl();
    }

    [ContextMenu("Setup All Planets ► Bowl Collision")]
    public void SetupPlanetsAndBowl()
    {
        // Resolve bowl reference
        if (bowlObject == null)
        {
            bowlObject = GameObject.Find("bowl_03");
            if (bowlObject == null)
            {
                Debug.LogError("MultiPlanetBowlSetup: Could not find bowl_03 in the scene – please assign the bowl object manually.");
                return;
            }
        }

        // Find planets automatically if requested
        if (autoFindPlanetsByName)
        {
            foreach (GameObject root in SceneManager.GetActiveScene().GetRootGameObjects())
            {
                foreach (Transform t in root.GetComponentsInChildren<Transform>(true))
                {
                    if (t.name.StartsWith(planetNamePrefix) && !planetObjects.Contains(t.gameObject))
                    {
                        planetObjects.Add(t.gameObject);
                    }
                }
            }
        }

        // Safeguard
        if (planetObjects.Count == 0)
        {
            Debug.LogWarning("MultiPlanetBowlSetup: No planet objects found – nothing to set up.");
            return;
        }

        Debug.Log($"▶ Setting up {planetObjects.Count} planets for bowl collision …");

        // Configure bowl once
        ConfigureBowlPhysics();

        // Configure each planet
        foreach (GameObject planet in planetObjects)
        {
            if (planet == null) continue;
            SetupSinglePlanet(planet);
        }

        Debug.Log("✓ Multi-planet setup complete!");
    }

    void ConfigureBowlPhysics()
    {
        Rigidbody bowlRb = bowlObject.GetComponent<Rigidbody>();

        if (makeBowlStatic)
        {
            // Ensure no Rigidbody so we can use a non-convex MeshCollider
            if (bowlRb != null)
            {
                if (Application.isPlaying) Destroy(bowlRb); else DestroyImmediate(bowlRb);
                bowlRb = null;
            }

            MeshCollider meshCol = bowlObject.GetComponent<MeshCollider>();
            if (meshCol == null) meshCol = bowlObject.AddComponent<MeshCollider>();
            meshCol.convex = false;
            meshCol.isTrigger = false;
        }
        else
        {
            if (bowlRb == null) bowlRb = bowlObject.AddComponent<Rigidbody>();
            bowlRb.isKinematic = false;
            bowlRb.useGravity = true;
            MeshCollider meshCol = bowlObject.GetComponent<MeshCollider>();
            if (meshCol == null) meshCol = bowlObject.AddComponent<MeshCollider>();
            meshCol.convex = true; // Rigidbody on same GameObject requires convex
            meshCol.isTrigger = false;
        }

        // Ensure tilt controller present so the bowl can rotate
        if (bowlObject.GetComponent<BowlTiltController>() == null)
        {
            bowlObject.AddComponent<BowlTiltController>();
        }
    }

    void SetupSinglePlanet(GameObject planetObj)
    {
        if (forceProjectGravity && Physics.gravity.sqrMagnitude < 0.01f)
        {
            Physics.gravity = new Vector3(0, -9.81f, 0);
        }

        // Rigidbody
        Rigidbody rb = planetObj.GetComponent<Rigidbody>();
        if (rb == null)
        {
            rb = planetObj.AddComponent<Rigidbody>();
        }
        rb.mass = planetMass;
        // The custom scripts in this project use linearDamping / angularDamping extension properties, so keep them if available.
        // Fallback to built-in drag properties when running in vanilla Unity builds.
        #if UNITY_2023_1_OR_NEWER || UNITY_2022_2_OR_NEWER
        // Newer Unity versions expose both APIs
        rb.linearDamping = planetDrag;
        rb.angularDamping = planetAngularDrag;
        #else
        rb.drag = planetDrag;
        rb.angularDrag = planetAngularDrag;
        #endif
        rb.useGravity = true;
        rb.isKinematic = false;
        rb.collisionDetectionMode = CollisionDetectionMode.Continuous;
        rb.interpolation = RigidbodyInterpolation.Interpolate;

        // Collider
        Collider col = planetObj.GetComponent<Collider>();
        if (col == null)
        {
            SphereCollider sc = planetObj.AddComponent<SphereCollider>();
            sc.radius = 0.5f;
            col = sc;
        }
        col.isTrigger = false;

        // Planet-to-bowl collision script
        PlanetBowlCollision collision = planetObj.GetComponent<PlanetBowlCollision>();
        if (collision == null) collision = planetObj.AddComponent<PlanetBowlCollision>();
        collision.SetBowlObject(bowlObject);
    }
}
