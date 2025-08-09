using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

/// <summary>
/// Spawns random planet prefabs when the player presses Space.
/// Maintains at most <see cref="maxPlanets"/> simultaneous planets by destroying the oldest ones.
/// </summary>
public class PlanetSpawner : MonoBehaviour
{
    [Header("Setup")]
    [Tooltip("Planet prefabs that can be spawned. Assign prefabs from the project.")]
    public List<GameObject> planetPrefabs = new List<GameObject>();

    [Tooltip("Optional audio clips matching planetPrefabs list 1:1. If provided, index i maps to planetPrefabs[i]")]
    public List<AudioClip> planetClips = new List<AudioClip>();

    [Tooltip("Reference to the bowl GameObject. If left empty, the spawner will try to find 'bowl_03' at runtime.")]
    public GameObject bowlObject;

    [Header("Spawn Settings")]
    [Tooltip("Maximum number of planets that can exist simultaneously.")]
    public int maxPlanets = 15;

    [Tooltip("Vertical offset from the bowl centre where planets appear.")]
    public float spawnHeight = 2f;

    [Tooltip("Initial linear damping applied to spawned planet Rigidbodies.")]
    public float planetDrag = 0.5f;

    [Tooltip("Initial angular damping applied to spawned planet Rigidbodies.")]
    public float planetAngularDrag = 0.5f;

    [Tooltip("Initial planet mass.")]
    public float planetMass = 1f;

    // Internal book-keeping of active planets (queue to easily drop the oldest)
    private readonly Queue<GameObject> activePlanets = new Queue<GameObject>();

    void Awake()
    {
        // Attempt auto-find if not assigned
        if (bowlObject == null)
        {
            bowlObject = GameObject.Find("bowl_03");
        }
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            SpawnRandomPlanet();
        }
        if (Input.GetKeyDown(KeyCode.R))
        {
            RemoveOnePlanet();
        }
    }

    void SpawnRandomPlanet()
    {
        if (planetPrefabs == null || planetPrefabs.Count == 0)
        {
            Debug.LogWarning("PlanetSpawner: No planet prefabs assigned – nothing to spawn.");
            return;
        }

        // Choose random prefab
        var prefabIndex = Random.Range(0, planetPrefabs.Count);
        var prefab = planetPrefabs[prefabIndex];
        if (prefab == null)
        {
            Debug.LogWarning("PlanetSpawner: Encountered null prefab reference.");
            return;
        }

        // Determine spawn position above the bowl (or world origin if bowl not found)
        Vector3 centre = bowlObject != null ? bowlObject.transform.position : Vector3.zero;
        Vector3 spawnPos = centre + Vector3.up * spawnHeight;
        Quaternion spawnRot = Random.rotation;

        GameObject planet = Instantiate(prefab, spawnPos, spawnRot);
        planet.name = prefab.name + "_Instance";

        // Configure physics & collision behaviour to match Planet_69
        SetupPlanetPhysics(planet);

        // Attach audio
        if (planetClips != null && prefabIndex < planetClips.Count && planetClips[prefabIndex] != null)
        {
            var pa = planet.GetComponent<PlanetAudio>();
            if (pa == null) pa = planet.AddComponent<PlanetAudio>();
            pa.collisionClip = planetClips[prefabIndex];
        }

        // Enqueue and enforce limit
        activePlanets.Enqueue(planet);
        if (activePlanets.Count > Mathf.Max(1, maxPlanets))
        {
            GameObject oldest = activePlanets.Dequeue();
            if (oldest != null)
            {
                Destroy(oldest);
            }
        }
    }

    void SetupPlanetPhysics(GameObject planet)
    {
        // Rigidbody
        var rb = planet.GetComponent<Rigidbody>();
        if (rb == null) rb = planet.AddComponent<Rigidbody>();
    #if UNITY_2023_1_OR_NEWER || UNITY_2022_2_OR_NEWER
        rb.linearDamping = planetDrag;
        rb.angularDamping = planetAngularDrag;
    #else
        rb.drag = planetDrag;
        rb.angularDrag = planetAngularDrag;
    #endif
        rb.mass = planetMass;
        rb.useGravity = true;
        rb.collisionDetectionMode = CollisionDetectionMode.Continuous;
        rb.interpolation = RigidbodyInterpolation.Interpolate;

        // Collider
        var col = planet.GetComponent<Collider>();
        if (col == null)
        {
            var sc = planet.AddComponent<SphereCollider>();
            sc.radius = 0.5f;
            col = sc;
        }
        col.isTrigger = false;

        // Collision helper script
        var collision = planet.GetComponent<PlanetBowlCollision>();
        if (collision == null) collision = planet.AddComponent<PlanetBowlCollision>();
        collision.SetBowlObject(bowlObject);
    }

    void ClearAllPlanets()
    {
        while (activePlanets.Count > 0)
        {
            var planet = activePlanets.Dequeue();
            if (planet != null)
            {
                Destroy(planet);
            }
        }
    }

    void RemoveOnePlanet()
    {
        if (activePlanets.Count == 0) return;
        var planet = activePlanets.Dequeue();
        if (planet != null)
        {
            Destroy(planet);
        }
    }
}
