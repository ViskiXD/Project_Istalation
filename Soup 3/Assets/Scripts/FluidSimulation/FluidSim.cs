using System;
using UnityEngine;
using Seb.GPUSorting;
using Unity.Mathematics;
using System.Collections.Generic;
using Seb.Helpers;
using static Seb.Helpers.ComputeHelper;

namespace Seb.Fluid.Simulation
{
    public class FluidSim : MonoBehaviour
    {
        public event Action<FluidSim> SimulationInitCompleted;

        [Header("Time Step")] public float normalTimeScale = 1;
        public float slowTimeScale = 0.1f;
        public float maxTimestepFPS = 60; // if time-step dips lower than this fps, simulation will run slower (set to 0 to disable)
        public int iterationsPerFrame = 3;

        [Header("Simulation Settings")] public float gravity = -10;
        public float smoothingRadius = 0.2f;
        public float targetDensity = 630;
        public float pressureMultiplier = 288;
        public float nearPressureMultiplier = 2.15f;
        public float viscosityStrength = 0;

        public enum BoundaryShape { Cube, Cylinder, TaperedCylinder }
        public BoundaryShape boundaryShape = BoundaryShape.Cube;
        public float boundaryRadiusBottomNorm = 0.5f; // normalised (0.5 == halfSize)
        public float boundaryRadiusTopNorm = 0.5f;
        [Range(0, 1)] public float collisionDamping = 0.95f;

        [Header("Foam Settings")] public bool foamActive;
        public int maxFoamParticleCount = 1000;
        public float trappedAirSpawnRate = 70;
        public float spawnRateFadeInTime = 0.5f;
        public float spawnRateFadeStartTime = 0;
        public Vector2 trappedAirVelocityMinMax = new(5, 25);
        public Vector2 foamKineticEnergyMinMax = new(15, 80);
        public float bubbleBuoyancy = 1.5f;
        public int sprayClassifyMaxNeighbours = 5;
        public int bubbleClassifyMinNeighbours = 15;
        public float bubbleScale = 0.5f;
        public float bubbleChangeScaleSpeed = 7;

        [Header("Volumetric Render Settings")] public bool renderToTex3D;
        public int densityTextureRes;

        [Header("References")] public ComputeShader compute;
        public Spawner3D spawner;

        [HideInInspector] public RenderTexture DensityMap;
        public Vector3 Scale => transform.localScale;
        
        // Buffers
        public ComputeBuffer foamBuffer { get; private set; }
        public ComputeBuffer foamSortTargetBuffer { get; private set; }
        public ComputeBuffer foamCountBuffer { get; private set; }
        public ComputeBuffer positionBuffer { get; private set; }
        public ComputeBuffer velocityBuffer { get; private set; }
        public ComputeBuffer densityBuffer { get; private set; }
        public int numParticles => positionBuffer?.count ?? 0;
        public ComputeBuffer predictedPositionsBuffer;
        public ComputeBuffer debugBuffer { get; private set; }

        ComputeBuffer sortTarget_positionBuffer;
        ComputeBuffer sortTarget_velocityBuffer;
        ComputeBuffer sortTarget_predictedPositionsBuffer;

        // Kernel IDs (match reference compute shader)
        const int externalForcesKernel = 0;
        const int spatialHashKernel = 1;
        const int reorderKernel = 2;
        const int reorderCopybackKernel = 3;
        const int densityKernel = 4;
        const int pressureKernel = 5;
        const int viscosityKernel = 6;
        const int updatePositionsKernel = 7;
        const int renderKernel = 8;
        const int foamUpdateKernel = 9;
        const int foamReorderCopyBackKernel = 10;

        SpatialHash spatialHash;

        // State
        bool isPaused;
        bool pauseNextFrame;
        float smoothRadiusOld;
        float simTimer;
        bool inSlowMode;
        Spawner3D.SpawnData spawnData;
        Dictionary<ComputeBuffer, string> bufferNameLookup;
        
        void Start()
        {
            // Load compute from Resources if not assigned
            if (compute == null)
            {
                compute = Resources.Load<ComputeShader>("FluidSim");
            }
            isPaused = false;
            Initialize();
        }

        void Initialize()
        {
            spawnData = spawner.GetSpawnData();
            int numParticles = spawnData.points.Length;

            spatialHash = new SpatialHash(numParticles);
            
            // Create buffers
            positionBuffer = CreateStructuredBuffer<float3>(numParticles);
            predictedPositionsBuffer = CreateStructuredBuffer<float3>(numParticles);
            velocityBuffer = CreateStructuredBuffer<float3>(numParticles);
            densityBuffer = CreateStructuredBuffer<float2>(numParticles);
            foamBuffer = CreateStructuredBuffer<FoamParticle>(maxFoamParticleCount);
            foamSortTargetBuffer = CreateStructuredBuffer<FoamParticle>(maxFoamParticleCount);
            foamCountBuffer = CreateStructuredBuffer<uint>(4096);
            debugBuffer = CreateStructuredBuffer<float3>(numParticles);

            sortTarget_positionBuffer = CreateStructuredBuffer<float3>(numParticles);
            sortTarget_predictedPositionsBuffer = CreateStructuredBuffer<float3>(numParticles);
            sortTarget_velocityBuffer = CreateStructuredBuffer<float3>(numParticles);

            bufferNameLookup = new Dictionary<ComputeBuffer, string>
            {
                { positionBuffer, "Positions" },
                { predictedPositionsBuffer, "PredictedPositions" },
                { velocityBuffer, "Velocities" },
                { densityBuffer, "Densities" },
                { spatialHash.SpatialKeys, "SpatialKeys" },
                { spatialHash.SpatialOffsets, "SpatialOffsets" },
                { spatialHash.SpatialIndices, "SortedIndices" },
                { sortTarget_positionBuffer, "SortTarget_Positions" },
                { sortTarget_predictedPositionsBuffer, "SortTarget_PredictedPositions" },
                { sortTarget_velocityBuffer, "SortTarget_Velocities" },
                { foamCountBuffer, "WhiteParticleCounters" },
                { foamBuffer, "WhiteParticles" },
                { foamSortTargetBuffer, "WhiteParticlesCompacted" },
                { debugBuffer, "Debug" }
            };

            // Set initial data
            SetInitialBufferData(spawnData);

            // Bind buffers per kernel
            SetBuffers(compute, externalForcesKernel, bufferNameLookup, new ComputeBuffer[]
            {
                positionBuffer,
                predictedPositionsBuffer,
                velocityBuffer
            });

            SetBuffers(compute, spatialHashKernel, bufferNameLookup, new ComputeBuffer[]
            {
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets,
                predictedPositionsBuffer,
                spatialHash.SpatialIndices
            });

            SetBuffers(compute, reorderKernel, bufferNameLookup, new ComputeBuffer[]
            {
                positionBuffer,
                sortTarget_positionBuffer,
                predictedPositionsBuffer,
                sortTarget_predictedPositionsBuffer,
                velocityBuffer,
                sortTarget_velocityBuffer,
                spatialHash.SpatialIndices
            });

            SetBuffers(compute, reorderCopybackKernel, bufferNameLookup, new ComputeBuffer[]
            {
                positionBuffer,
                sortTarget_positionBuffer,
                predictedPositionsBuffer,
                sortTarget_predictedPositionsBuffer,
                velocityBuffer,
                sortTarget_velocityBuffer,
                spatialHash.SpatialIndices
            });

            SetBuffers(compute, densityKernel, bufferNameLookup, new ComputeBuffer[]
            {
                predictedPositionsBuffer,
                densityBuffer,
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets
            });

            SetBuffers(compute, pressureKernel, bufferNameLookup, new ComputeBuffer[]
            {
                predictedPositionsBuffer,
                densityBuffer,
                velocityBuffer,
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets,
                foamBuffer,
                foamCountBuffer,
                debugBuffer
            });

            SetBuffers(compute, viscosityKernel, bufferNameLookup, new ComputeBuffer[]
            {
                predictedPositionsBuffer,
                densityBuffer,
                velocityBuffer,
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets
            });

            SetBuffers(compute, updatePositionsKernel, bufferNameLookup, new ComputeBuffer[]
            {
                positionBuffer,
                velocityBuffer
            });

            SetBuffers(compute, renderKernel, bufferNameLookup, new ComputeBuffer[]
            {
                predictedPositionsBuffer,
                densityBuffer,
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets,
            });

            SetBuffers(compute, foamUpdateKernel, bufferNameLookup, new ComputeBuffer[]
            {
                foamBuffer,
                foamCountBuffer,
                predictedPositionsBuffer,
                densityBuffer,
                velocityBuffer,
                spatialHash.SpatialKeys,
                spatialHash.SpatialOffsets,
                foamSortTargetBuffer,
            });

            SetBuffers(compute, foamReorderCopyBackKernel, bufferNameLookup, new ComputeBuffer[]
            {
                foamBuffer,
                foamSortTargetBuffer,
                foamCountBuffer,
            });

            compute.SetInt("numParticles", positionBuffer.count);
            compute.SetInt("MaxWhiteParticleCount", maxFoamParticleCount);

            UpdateSmoothingConstants();

            // Initialize density texture if needed
            if (renderToTex3D)
            {
                RunSimulationFrame(0);
            }

            SimulationInitCompleted?.Invoke(this);
        }
        
        void Update()
        {
            if (!isPaused)
            {
                float maxDeltaTime = maxTimestepFPS > 0 ? 1 / maxTimestepFPS : float.PositiveInfinity;
                float dt = Mathf.Min(Time.deltaTime * ActiveTimeScale, maxDeltaTime);
                RunSimulationFrame(dt);
            }

            if (pauseNextFrame)
            {
                isPaused = true;
                pauseNextFrame = false;
            }
        }

        void RunSimulationFrame(float frameDeltaTime)
        {
            float subStepDeltaTime = frameDeltaTime / iterationsPerFrame;
            UpdateSettings(subStepDeltaTime, frameDeltaTime);

            for (int i = 0; i < iterationsPerFrame; i++)
            {
                simTimer += subStepDeltaTime;
                RunSimulationStep();
            }
        }

        void RunSimulationStep()
        {
            Dispatch(compute, positionBuffer.count, kernelIndex: externalForcesKernel);
            Dispatch(compute, positionBuffer.count, kernelIndex: spatialHashKernel);
            spatialHash.Run();
            Dispatch(compute, positionBuffer.count, kernelIndex: reorderKernel);
            Dispatch(compute, positionBuffer.count, kernelIndex: reorderCopybackKernel);
            Dispatch(compute, positionBuffer.count, kernelIndex: densityKernel);
            Dispatch(compute, positionBuffer.count, kernelIndex: pressureKernel);
            if (viscosityStrength != 0) Dispatch(compute, positionBuffer.count, kernelIndex: viscosityKernel);
            Dispatch(compute, positionBuffer.count, kernelIndex: updatePositionsKernel);

            // ---- Foam (white particles) ----
            if (foamActive)
            {
                // Update foam particles (simulate, classify, compact)
                int threadGroups = Mathf.CeilToInt(maxFoamParticleCount / 256f);
                compute.Dispatch(foamUpdateKernel, (int)threadGroups, 1, 1);
                compute.Dispatch(foamReorderCopyBackKernel, (int)threadGroups, 1, 1);
            }
        }

        void UpdateSmoothingConstants()
        {
            float r = smoothingRadius;
            float spikyPow2 = 15 / (2 * Mathf.PI * Mathf.Pow(r, 5));
            float spikyPow3 = 15 / (Mathf.PI * Mathf.Pow(r, 6));
            float spikyPow2Grad = 15 / (Mathf.PI * Mathf.Pow(r, 5));
            float spikyPow3Grad = 45 / (Mathf.PI * Mathf.Pow(r, 6));

            compute.SetFloat("K_SpikyPow2", spikyPow2);
            compute.SetFloat("K_SpikyPow3", spikyPow3);
            compute.SetFloat("K_SpikyPow2Grad", spikyPow2Grad);
            compute.SetFloat("K_SpikyPow3Grad", spikyPow3Grad);
        }

        void UpdateSettings(float stepDeltaTime, float frameDeltaTime)
        {
            if (smoothingRadius != smoothRadiusOld)
            {
                smoothRadiusOld = smoothingRadius;
                UpdateSmoothingConstants();
            }

            Vector3 simBoundsSize = transform.localScale;
            Vector3 simBoundsCentre = transform.position;

            compute.SetFloat("deltaTime", stepDeltaTime);
            compute.SetFloat("whiteParticleDeltaTime", frameDeltaTime);
            compute.SetFloat("simTime", simTimer);
            compute.SetFloat("gravity", gravity);
            compute.SetVector("gravityVec", Vector3.down * Mathf.Abs(gravity));
            compute.SetFloat("collisionDamping", collisionDamping);
            compute.SetFloat("smoothingRadius", smoothingRadius);
            compute.SetFloat("targetDensity", targetDensity);
            compute.SetFloat("pressureMultiplier", pressureMultiplier);
            compute.SetFloat("nearPressureMultiplier", nearPressureMultiplier);
            compute.SetFloat("viscosityStrength", viscosityStrength);
            compute.SetVector("boundarySize", simBoundsSize);
            compute.SetVector("boundaryCenter", simBoundsCentre);
            compute.SetInt("boundaryShape", (int)boundaryShape);
            compute.SetFloat("radiusBottom", boundaryRadiusBottomNorm);
            compute.SetFloat("radiusTop", boundaryRadiusTopNorm);

            compute.SetMatrix("localToWorld", transform.localToWorldMatrix);
            compute.SetMatrix("worldToLocal", transform.worldToLocalMatrix);

            // Foam settings
            float fadeInT = (spawnRateFadeInTime <= 0) ? 1 : Mathf.Clamp01((simTimer - spawnRateFadeStartTime) / spawnRateFadeInTime);
            compute.SetVector("trappedAirParams", new Vector3(trappedAirSpawnRate * fadeInT * fadeInT, trappedAirVelocityMinMax.x, trappedAirVelocityMinMax.y));
            compute.SetVector("kineticEnergyParams", foamKineticEnergyMinMax);
            compute.SetFloat("bubbleBuoyancy", bubbleBuoyancy);
            compute.SetInt("sprayClassifyMaxNeighbours", sprayClassifyMaxNeighbours);
            compute.SetInt("bubbleClassifyMinNeighbours", bubbleClassifyMinNeighbours);
            compute.SetFloat("bubbleScaleChangeSpeed", bubbleChangeScaleSpeed);
            compute.SetFloat("bubbleScale", bubbleScale);
        }

        void SetInitialBufferData(Spawner3D.SpawnData spawnData)
        {
            positionBuffer.SetData(spawnData.points);
            predictedPositionsBuffer.SetData(spawnData.points);
            velocityBuffer.SetData(spawnData.velocities);

            foamBuffer.SetData(new FoamParticle[foamBuffer.count]);

            debugBuffer.SetData(new float3[debugBuffer.count]);
            foamCountBuffer.SetData(new uint[foamCountBuffer.count]);
            simTimer = 0;
        }

        private float ActiveTimeScale => inSlowMode ? slowTimeScale : normalTimeScale;

        void OnDestroy()
        {
            foreach (var kvp in bufferNameLookup)
            {
                Release(kvp.Key);
            }

            spatialHash.Release();
        }

        public struct FoamParticle
        {
            public float3 position;
            public float3 velocity;
            public float lifetime;
            public float scale;
        }

        void OnDrawGizmos()
        {
            // Draw Bounds
            var m = Gizmos.matrix;
            Gizmos.matrix = transform.localToWorldMatrix;
            Gizmos.color = new Color(0, 1, 0, 0.5f);
            Gizmos.DrawWireCube(Vector3.zero, Vector3.one);
            Gizmos.matrix = m;
        }
    }
}