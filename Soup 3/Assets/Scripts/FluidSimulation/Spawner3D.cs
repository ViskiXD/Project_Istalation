using System;
using System.Collections.Generic;
using UnityEngine;

namespace Seb.Fluid.Simulation
{
    public class Spawner3D : MonoBehaviour
    {
        public enum Shape { Cube, Cylinder, TaperedCylinder }

        public int particleSpawnDensity = 600;
        public Vector3 initialVel;
        public float jitterStrength;
        public bool showSpawnBounds;
        public SpawnRegion[] spawnRegions;

        [Header("Debug Info")] 
        public int debug_num_particles;
        public float debug_spawn_volume;

        public SpawnData GetSpawnData()
        {
            List<Vector3> allPoints = new();
            List<Vector3> allVelocities = new();

            if (spawnRegions != null)
            {
                foreach (SpawnRegion region in spawnRegions)
                {
                    int particlesPerAxis = region.CalculateParticleCountPerAxis(particleSpawnDensity);
                    Vector3[] points;
                    Vector3[] velocities;
                    switch (region.shape)
                    {
                        case Shape.Cylinder:
                            (points, velocities) = SpawnCylinder(particlesPerAxis, region.centre, region.size);
                            break;
                        case Shape.TaperedCylinder:
                            (points, velocities) = SpawnTaperedCylinder(particlesPerAxis, region.centre, region.size);
                            break;
                        default:
                            (points, velocities) = SpawnCube(particlesPerAxis, region.centre, region.size);
                            break;
                    }
                    allPoints.AddRange(points);
                    allVelocities.AddRange(velocities);
                }
            }

            return new SpawnData() { points = allPoints.ToArray(), velocities = allVelocities.ToArray() };
        }

        (Vector3[] p, Vector3[] v) SpawnCube(int numPerAxis, Vector3 centre, Vector3 size)
        {
            int numPoints = numPerAxis * numPerAxis * numPerAxis;
            Vector3[] points = new Vector3[numPoints];
            Vector3[] velocities = new Vector3[numPoints];

            int i = 0;

            if (numPerAxis <= 1)
            {
                // Single particle at centre
                points[0] = centre;
                velocities[0] = initialVel;
                return (points, velocities);
            }

            float denom = Mathf.Max(1e-6f, (numPerAxis - 1f));
            for (int x = 0; x < numPerAxis; x++)
            {
                for (int y = 0; y < numPerAxis; y++)
                {
                    for (int z = 0; z < numPerAxis; z++)
                    {
                        float tx = x / denom;
                        float ty = y / denom;
                        float tz = z / denom;

                        float px = (tx - 0.5f) * size.x + centre.x;
                        float py = (ty - 0.5f) * size.y + centre.y;
                        float pz = (tz - 0.5f) * size.z + centre.z;
                        Vector3 jitter = UnityEngine.Random.insideUnitSphere * jitterStrength;
                        points[i] = new Vector3(px, py, pz) + jitter * 0.25f;
                        velocities[i] = initialVel;
                        i++;
                    }
                }
            }

            return (points, velocities);
        }

        (Vector3[] p, Vector3[] v) SpawnCylinder(int numPerAxis, Vector3 centre, Vector3 size)
        {
            float radius = size.x * 0.5f;
            float height = size.y;
            List<Vector3> pointList = new();
            List<Vector3> velList = new();

            if (numPerAxis <= 1)
            {
                pointList.Add(centre);
                velList.Add(initialVel);
                return (pointList.ToArray(), velList.ToArray());
            }

            float denom = Mathf.Max(1e-6f, numPerAxis - 1f);
            for (int x = 0; x < numPerAxis; x++)
            {
                for (int y = 0; y < numPerAxis; y++)
                {
                    for (int z = 0; z < numPerAxis; z++)
                    {
                        float tx = x / denom;
                        float ty = y / denom;
                        float tz = z / denom;

                        float px = (tx - 0.5f) * radius * 2 + centre.x;
                        float py = (ty - 0.5f) * height + centre.y;
                        float pz = (tz - 0.5f) * radius * 2 + centre.z;

                        Vector2 horiz = new Vector2(px - centre.x, pz - centre.z);
                        if (horiz.sqrMagnitude <= radius * radius)
                        {
                            Vector3 jitter = UnityEngine.Random.insideUnitSphere * jitterStrength * 0.25f;
                            pointList.Add(new Vector3(px, py, pz) + jitter);
                            velList.Add(initialVel);
                        }
                    }
                }
            }
            return (pointList.ToArray(), velList.ToArray());
        }

        (Vector3[] p, Vector3[] v) SpawnTaperedCylinder(int numPerAxis, Vector3 centre, Vector3 size)
        {
            float radiusBottom = size.x * 0.5f;
            float radiusTop = Mathf.Max(0.0001f, size.z * 0.5f);
            float height = size.y;

            List<Vector3> pointList = new();
            List<Vector3> velList = new();
            if (numPerAxis <= 1)
            {
                pointList.Add(centre);
                velList.Add(initialVel);
                return (pointList.ToArray(), velList.ToArray());
            }

            float denom = Mathf.Max(1e-6f, numPerAxis - 1f);
            for (int x = 0; x < numPerAxis; x++)
            {
                for (int y = 0; y < numPerAxis; y++)
                {
                    for (int z = 0; z < numPerAxis; z++)
                    {
                        float tx = x / denom;
                        float ty = y / denom;
                        float tz = z / denom;

                        float px = (tx - 0.5f) * radiusTop * 2 + centre.x; // use top diameter for grid extents
                        float py = (ty - 0.5f) * height + centre.y;
                        float pz = (tz - 0.5f) * radiusTop * 2 + centre.z;

                        float fracY = (py - (centre.y - height * 0.5f)) / height; // 0 at bottom,1 at top
                        float currRadius = Mathf.Lerp(radiusBottom, radiusTop, fracY);
                        Vector2 horiz = new Vector2(px - centre.x, pz - centre.z);
                        if (horiz.sqrMagnitude <= currRadius * currRadius)
                        {
                            Vector3 jitter = UnityEngine.Random.insideUnitSphere * jitterStrength * 0.25f;
                            pointList.Add(new Vector3(px, py, pz) + jitter);
                            velList.Add(initialVel);
                        }
                    }
                }
            }
            return (pointList.ToArray(), velList.ToArray());
        }

        void OnValidate()
        {
            debug_spawn_volume = 0;
            debug_num_particles = 0;

            if (spawnRegions != null)
            {
                foreach (SpawnRegion region in spawnRegions)
                {
                    debug_spawn_volume += region.Volume;
                    int numPerAxis = region.CalculateParticleCountPerAxis(particleSpawnDensity);
                    debug_num_particles += numPerAxis * numPerAxis * numPerAxis;
                }
            }
        }

        void OnDrawGizmos()
        {
            if (showSpawnBounds && !Application.isPlaying)
            {
                if (spawnRegions != null)
                {
                    foreach (SpawnRegion region in spawnRegions)
                    {
                        Gizmos.color = region.debugDisplayCol;
                        if (region.shape == Shape.Cylinder || region.shape==Shape.TaperedCylinder)
                        {
                            // Approximate cylinder/taper with multiple circles
                            int segments = 20;
                            float r = region.size.x * 0.5f;
                            float h = region.size.y;
                            Vector3 up = Vector3.up * h * 0.5f;
                            for (int s = 0; s < segments; s++)
                            {
                                float a0 = (s / (float)segments) * Mathf.PI * 2;
                                float a1 = ((s + 1) / (float)segments) * Mathf.PI * 2;
                                Vector3 p0 = region.centre + new Vector3(Mathf.Cos(a0) * r, -up.y, Mathf.Sin(a0) * r);
                                Vector3 p1 = region.centre + new Vector3(Mathf.Cos(a1) * r, -up.y, Mathf.Sin(a1) * r);
                                Vector3 p2 = p0 + Vector3.up * h;
                                Vector3 p3 = p1 + Vector3.up * h;
                                Gizmos.DrawLine(p0, p1);
                                Gizmos.DrawLine(p2, p3);
                                Gizmos.DrawLine(p0, p2);
                            }
                        }
                        else
                        {
                            Gizmos.DrawWireCube(region.centre, region.size);
                        }
                    }
                }
            }
        }

        [System.Serializable]
        public struct SpawnRegion
        {
            public Vector3 centre;
            public Vector3 size; // for cube: full extents. for cylinder: x = diameter, y = height, z ignored
            public Shape shape;
            public Color debugDisplayCol;

            public float Volume
            {
                get
                {
                    switch (shape)
                    {
                        case Shape.Cylinder:
                            {
                                float r = size.x * 0.5f;
                                return Mathf.PI * r * r * size.y;
                            }
                        case Shape.TaperedCylinder:
                            {
                                float r0 = size.x * 0.5f;
                                float r1 = Mathf.Max(0.0001f, size.z * 0.5f);
                                return Mathf.PI * size.y * (r0 * r0 + r0 * r1 + r1 * r1) / 3f;
                            }
                        default:
                            return size.x * size.y * size.z;
                    }
                }
            }

            public int CalculateParticleCountPerAxis(int particleDensity)
            {
                int targetParticleCount = Mathf.Max(1, (int)(Volume * particleDensity));
                int particlesPerAxis = Mathf.Max(1, (int)Mathf.Round(Mathf.Pow(targetParticleCount, 1f / 3f)));
                return Mathf.Max(1, particlesPerAxis);
            }
        }

        public struct SpawnData
        {
            public Vector3[] points;
            public Vector3[] velocities;
        }
    }
}