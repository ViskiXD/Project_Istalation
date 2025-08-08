using UnityEngine;
using UnityEngine.Experimental.Rendering;
using System.Collections.Generic;
using System;

namespace Seb.Helpers
{
    public enum DepthMode
    {
        None = 0,
        Depth16 = 16,
        Depth24 = 24
    }

    public static class ComputeHelper
    {
        public const FilterMode defaultFilterMode = FilterMode.Bilinear;
        public const GraphicsFormat defaultGraphicsFormat = GraphicsFormat.R32G32B32A32_SFloat;
        static readonly uint[] argsBufferArray = new uint[5];

        public static void Dispatch(ComputeShader cs, Vector3Int numIterations, int kernelIndex = 0)
        { Dispatch(cs, numIterations.x, numIterations.y, numIterations.z, kernelIndex); }

        public static void Dispatch(ComputeShader cs, int numIterationsX, int numIterationsY = 1, int numIterationsZ = 1, int kernelIndex = 0)
        {
            Vector3Int tgs = GetThreadGroupSizes(cs, kernelIndex);
            int gx = Mathf.CeilToInt(numIterationsX / (float)tgs.x);
            int gy = Mathf.CeilToInt(numIterationsY / (float)tgs.y);
            int gz = Mathf.CeilToInt(numIterationsZ / (float)tgs.z);
            cs.Dispatch(kernelIndex, gx, gy, gz);
        }

        public static int CalculateThreadGroupCount1D(ComputeShader cs, int numIterationsX, int kernelIndex = 0)
        { return Mathf.CeilToInt(numIterationsX / (float)GetThreadGroupSizes(cs, kernelIndex).x); }

        public static void Dispatch(ComputeShader cs, RenderTexture texture, int kernelIndex = 0)
        { Vector3Int tgs = GetThreadGroupSizes(cs, kernelIndex); Dispatch(cs, texture.width, texture.height, texture.volumeDepth, kernelIndex); }

        public static void Dispatch(ComputeShader cs, Texture2D texture, int kernelIndex = 0)
        { Vector3Int tgs = GetThreadGroupSizes(cs, kernelIndex); Dispatch(cs, texture.width, texture.height, 1, kernelIndex); }

        public static int GetStride<T>() { return System.Runtime.InteropServices.Marshal.SizeOf(typeof(T)); }

        public static ComputeBuffer CreateAppendBuffer<T>(int size = 1)
        { var b = new ComputeBuffer(size, GetStride<T>(), ComputeBufferType.Append); b.SetCounterValue(0); return b; }

        public static void CreateAppendBuffer<T>(ref ComputeBuffer buffer, int count)
        {
            int stride = GetStride<T>();
            bool createNew = buffer == null || !buffer.IsValid() || buffer.count != count || buffer.stride != stride;
            if (createNew) { Release(buffer); buffer = new ComputeBuffer(count, stride, ComputeBufferType.Append); }
            buffer.SetCounterValue(0);
        }

        public static bool CreateStructuredBuffer<T>(ref ComputeBuffer buffer, int count)
        {
            int stride = GetStride<T>();
            bool createNew = buffer == null || !buffer.IsValid() || buffer.count != count || buffer.stride != stride;
            if (createNew) { Release(buffer); buffer = new ComputeBuffer(count, stride); return true; }
            return false;
        }

        public static ComputeBuffer CreateStructuredBuffer<T>(int count) { return new ComputeBuffer(count, GetStride<T>()); }

        public static void SetBuffers(ComputeShader cs, int kernel, Dictionary<ComputeBuffer, string> nameLookup, params ComputeBuffer[] buffers)
        { foreach (var b in buffers) cs.SetBuffer(kernel, nameLookup[b], b); }

        public static void Release(params ComputeBuffer[] buffers)
        { foreach (var b in buffers) if (b != null) b.Release(); }

        public static Vector3Int GetThreadGroupSizes(ComputeShader compute, int kernelIndex = 0)
        { compute.GetKernelThreadGroupSizes(kernelIndex, out uint x, out uint y, out uint z); return new Vector3Int((int)x, (int)y, (int)z); }

        public static RenderTexture CreateRenderTexture(int width, int height, FilterMode filterMode, GraphicsFormat format, string name = "Unnamed", DepthMode depthMode = DepthMode.None, bool useMipMaps = false)
        {
            var tex = new RenderTexture(width, height, (int)depthMode) { graphicsFormat = format, enableRandomWrite = true, autoGenerateMips = false, useMipMap = useMipMaps };
            tex.Create(); tex.name = name; tex.wrapMode = TextureWrapMode.Clamp; tex.filterMode = filterMode; return tex;
        }

        public static void CreateRenderTexture3D(ref RenderTexture texture, int width, int height, int depth, GraphicsFormat format, TextureWrapMode wrapMode = TextureWrapMode.Repeat, string name = "Untitled", bool mipmaps = false)
        {
            if (texture == null || !texture.IsCreated() || texture.width != width || texture.height != height || texture.volumeDepth != depth || texture.graphicsFormat != format)
            {
                if (texture != null) texture.Release();
                texture = new RenderTexture(width, height, 0) { graphicsFormat = format, volumeDepth = depth, enableRandomWrite = true, dimension = UnityEngine.Rendering.TextureDimension.Tex3D, useMipMap = mipmaps, autoGenerateMips = false };
                texture.Create();
            }
            texture.wrapMode = wrapMode; texture.filterMode = FilterMode.Bilinear; texture.name = name;
        }

        public static void CreateArgsBuffer(ref ComputeBuffer argsBuffer, Mesh mesh, int numInstances)
        {
            const int stride = sizeof(uint); const int numArgs = 5; const int sub = 0;
            bool createNew = argsBuffer == null || !argsBuffer.IsValid() || argsBuffer.count != numArgs || argsBuffer.stride != stride;
            if (createNew) { Release(argsBuffer); argsBuffer = new ComputeBuffer(numArgs, stride, ComputeBufferType.IndirectArguments); }
            lock (argsBufferArray)
            {
                argsBufferArray[0] = (uint)mesh.GetIndexCount(sub);
                argsBufferArray[1] = (uint)numInstances;
                argsBufferArray[2] = (uint)mesh.GetIndexStart(sub);
                argsBufferArray[3] = (uint)mesh.GetBaseVertex(sub);
                argsBufferArray[4] = 0; argsBuffer.SetData(argsBufferArray);
            }
        }

        public static void LoadComputeShader(ref ComputeShader shader, string name)
        { if (shader == null) shader = LoadComputeShader(name); }

        public static ComputeShader LoadComputeShader(string name)
        { return Resources.Load<ComputeShader>(name.Split('.')[0]); }
    }
}

