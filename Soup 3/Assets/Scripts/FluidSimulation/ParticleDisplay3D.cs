using UnityEngine;
using Seb.Fluid.Simulation;

namespace Seb.Fluid.Rendering
{
    public class ParticleDisplay3D : MonoBehaviour
    {
        [Header("Rendering Mode")]
        public DisplayMode mode = DisplayMode.Billboard;
        
        [Header("Rendering Settings")]
        public float scale = 1f;
        public Gradient colourMap;
        public int gradientResolution = 64;
        public float velocityDisplayMax = 15f;
        public int meshResolution = 4;
        public bool useCpuInstancing = true;
        [Min(1)] public int renderStride = 2; // draw every Nth particle to reduce cost
        [Min(128)] public int maxInstances = 20000; // global cap for draw calls
        
        [Header("Shaders")]
        public Shader shaderShaded;
        public Shader shaderBillboard;
        
        [Header("Simulation Reference")]
        public FluidSim sim;
        
        public Mesh mesh { get; private set; }
        public Material mat { get; private set; }
        private Texture2D colourTexture;
        private ComputeBuffer argsBuffer;
        private Matrix4x4[] matrixBuffer;
        private Vector3[] positionsCache;
        
        public enum DisplayMode
        {
            Billboard,
            Mesh3D
        }
        
        void Start()
        {
            if (sim == null)
            {
                sim = FindFirstObjectByType<FluidSim>();
                if (sim == null)
                {
                    Debug.LogError("ParticleDisplay3D: No FluidSim found!");
                    return;
                }
            }
            
            InitializeRendering();
        }
        
        void InitializeRendering()
        {
            CreateMesh();
            CreateMaterial();
            CreateColourTexture();
            if (!useCpuInstancing)
            {
                CreateArgsBuffer();
                // For indirect path, make sure we use the reference billboard shader
                Shader indirectShader = Shader.Find("Fluid/ParticleBillboard");
                if (indirectShader != null && (mat == null || mat.shader != indirectShader))
                {
                    mat = new Material(indirectShader);
                    mat.enableInstancing = true;
                }
            }
            else
            {
                matrixBuffer = new Matrix4x4[1023];
            }
        }
        
        void CreateMesh()
        {
            if (mode == DisplayMode.Mesh3D)
            {
                mesh = CreateSphereMesh();
            }
            else
            {
                mesh = CreateQuadMesh();
            }
        }
        
        Mesh CreateQuadMesh()
        {
            Mesh quadMesh = new Mesh();
            quadMesh.name = "Billboard Quad";
            
            Vector3[] vertices = new Vector3[]
            {
                new Vector3(-0.5f, -0.5f, 0),
                new Vector3(0.5f, -0.5f, 0),
                new Vector3(-0.5f, 0.5f, 0),
                new Vector3(0.5f, 0.5f, 0)
            };
            
            Vector2[] uvs = new Vector2[]
            {
                new Vector2(0, 0),
                new Vector2(1, 0),
                new Vector2(0, 1),
                new Vector2(1, 1)
            };
            
            int[] triangles = new int[] { 0, 2, 1, 2, 3, 1 };
            
            quadMesh.vertices = vertices;
            quadMesh.uv = uvs;
            quadMesh.triangles = triangles;
            quadMesh.RecalculateNormals();
            
            return quadMesh;
        }
        
        void CreateMaterial()
        {
            Shader targetShader = null;
            if (mode == DisplayMode.Billboard)
            {
                targetShader = useCpuInstancing ? Shader.Find("Fluid/InstancedBillboardCircle") : Shader.Find("Fluid/ParticleBillboard");
            }
            else
            {
                targetShader = Shader.Find("Fluid/Particle3DSurf");
            }

            if (targetShader == null)
            {
                targetShader = Shader.Find("Universal Render Pipeline/Unlit")
                                ?? Shader.Find("Unlit/Color")
                                ?? Shader.Find("Standard")
                                ?? Shader.Find("Sprites/Default");
            }
            if (targetShader == null)
            {
                Debug.LogError("ParticleDisplay3D: No suitable shader found!");
                return;
            }

            mat = new Material(targetShader);
            mat.name = $"Particle Material ({mode})";
            mat.enableInstancing = true;
            Color lightBlueTrans = new Color(0.6f, 0.8f, 1f, 0.1f);
            if (mat.HasProperty("_BaseColor")) mat.SetColor("_BaseColor", lightBlueTrans);
            else if (mat.HasProperty("_Color")) mat.SetColor("_Color", lightBlueTrans);
        }
        
        void CreateColourTexture()
        {
            if (colourMap == null)
            {
                colourMap = new Gradient();
                GradientColorKey[] colorKeys = new GradientColorKey[2];
                Color lightBlue = new Color(0.6f, 0.8f, 1f);
                colorKeys[0] = new GradientColorKey(lightBlue, 0f);
                colorKeys[1] = new GradientColorKey(lightBlue, 1f);
                
                GradientAlphaKey[] alphaKeys = new GradientAlphaKey[2];
                alphaKeys[0] = new GradientAlphaKey(0.1f, 0f);
                alphaKeys[1] = new GradientAlphaKey(0.1f, 1f);
                
                colourMap.SetKeys(colorKeys, alphaKeys);
            }
            
            colourTexture = new Texture2D(gradientResolution, 1, TextureFormat.RGBA32, false);
            colourTexture.wrapMode = TextureWrapMode.Clamp;
            
            Color[] colors = new Color[gradientResolution];
            for (int i = 0; i < gradientResolution; i++)
            {
                float t = i / (float)(gradientResolution - 1);
                colors[i] = colourMap.Evaluate(t);
            }
            
            colourTexture.SetPixels(colors);
            colourTexture.Apply();
        }
        
        void CreateArgsBuffer()
        {
            if (mesh == null) return;
            
            argsBuffer = new ComputeBuffer(5, sizeof(uint), ComputeBufferType.IndirectArguments);
            
            uint[] args = new uint[]
            {
                (uint)mesh.GetIndexCount(0),
                0,
                (uint)mesh.GetIndexStart(0),
                (uint)mesh.GetBaseVertex(0),
                0
            };
            
            argsBuffer.SetData(args);
        }
        
        void Update()
        {
            if (sim == null || sim.positionBuffer == null || mat == null || mesh == null)
                return;

            if (useCpuInstancing)
            {
                RenderParticlesCpuBillboards();
            }
            else
            {
                if (argsBuffer == null) { CreateArgsBuffer(); }

                if (mode == DisplayMode.Billboard)
                {
                    if (mat.shader.name != "Fluid/ParticleBillboard")
                    {
                        Shader s = Shader.Find("Fluid/ParticleBillboard");
                        if (s != null)
                        {
                            mat = new Material(s);
                            mat.enableInstancing = true;
                        }
                    }
                    if (mat.shader.name == "Fluid/ParticleBillboard")
                    {
                        mat.SetBuffer("Positions", sim.positionBuffer);
                        if (sim.velocityBuffer != null) mat.SetBuffer("Velocities", sim.velocityBuffer);
                        if (colourTexture != null) mat.SetTexture("ColourMap", colourTexture);
                        mat.SetFloat("scale", Mathf.Max(0.01f, scale));
                        mat.SetFloat("velocityMax", Mathf.Max(0.001f, velocityDisplayMax));
                    }
                }
                else if (mode == DisplayMode.Mesh3D)
                {
                    if (mat.shader.name != "Fluid/Particle3DSurf")
                    {
                        Shader s = Shader.Find("Fluid/Particle3DSurf");
                        if (s != null)
                        {
                            mat = new Material(s);
                            mat.enableInstancing = true;
                        }
                    }
                    if (mat.shader.name == "Fluid/Particle3DSurf")
                    {
                        mat.SetBuffer("Positions", sim.positionBuffer);
                        if (sim.velocityBuffer != null) mat.SetBuffer("Velocities", sim.velocityBuffer);
                        if (colourTexture != null) mat.SetTexture("ColourMap", colourTexture);
                        mat.SetFloat("scale", Mathf.Max(0.01f, scale));
                        mat.SetFloat("velocityMax", Mathf.Max(0.001f, velocityDisplayMax));
                    }
                }
                RenderParticlesIndirect();
            }
        }
        
        static bool IsFinite(Vector3 v)
        {
            return float.IsFinite(v.x) && float.IsFinite(v.y) && float.IsFinite(v.z);
        }
        
        void RenderParticlesIndirect()
        {
            if (sim.numParticles == 0) return;
            if (argsBuffer == null) { CreateArgsBuffer(); if (argsBuffer == null) return; }
            
            uint[] args = new uint[5];
            argsBuffer.GetData(args);
            args[1] = (uint)sim.numParticles;
            argsBuffer.SetData(args);
            
            Bounds bounds = new Bounds(transform.position, Vector3.one * 1000f);
            Graphics.DrawMeshInstancedIndirect(mesh, 0, mat, bounds, argsBuffer);
        }
        
        void RenderParticlesCpuBillboards()
        {
            int count = sim.numParticles;
            if (count == 0) return;
            
            if (positionsCache == null || positionsCache.Length != count)
            {
                positionsCache = new Vector3[count];
            }
            sim.positionBuffer.GetData(positionsCache);
            
            int stride = Mathf.Max(1, renderStride);
            int totalToDraw = Mathf.Min(maxInstances, Mathf.CeilToInt(count / (float)stride));

            Quaternion rot = Camera.main ? Camera.main.transform.rotation : Quaternion.identity;
            Vector3 oneScale = Vector3.one * Mathf.Max(0.01f, scale);

            int emitted = 0;
            int srcBase = 0;
            while (emitted < totalToDraw)
            {
                int written = 0;
                int capacity = Mathf.Min(1023, totalToDraw - emitted);
                for (int i = 0; i < capacity; i++)
                {
                    int srcIndex = (emitted + srcBase + i) * stride;
                    if (srcIndex >= count) break;
                    Vector3 p = positionsCache[srcIndex];
                    if (!IsFinite(p) || p.sqrMagnitude > 1e12f) // skip NaN/Inf and absurd values
                        continue;
                    matrixBuffer[written++] = Matrix4x4.TRS(p, rot, oneScale);
                }

                if (written > 0)
                {
                    Graphics.DrawMeshInstanced(mesh, 0, mat, matrixBuffer, written);
                    emitted += written;
                }
                else
                {
                    // nothing valid found in this window; advance source window to avoid infinite loop
                    srcBase += capacity;
                    if ((emitted + srcBase) * stride >= count) break;
                }
            }
        }
        
        void OnDestroy()
        {
            if (mat != null)
            {
                if (Application.isPlaying)
                    Destroy(mat);
                else
                    DestroyImmediate(mat);
            }
            
            if (colourTexture != null)
            {
                if (Application.isPlaying)
                    Destroy(colourTexture);
                else
                    DestroyImmediate(colourTexture);
            }
            
            if (argsBuffer != null)
            {
                argsBuffer.Release();
                argsBuffer = null;
            }
        }

        Mesh CreateSphereMesh()
        {
            GameObject temp = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            Mesh sphere = temp.GetComponent<MeshFilter>().sharedMesh;
            if (Application.isPlaying)
                Destroy(temp);
            else
                DestroyImmediate(temp);
            return sphere;
        }
    }
}