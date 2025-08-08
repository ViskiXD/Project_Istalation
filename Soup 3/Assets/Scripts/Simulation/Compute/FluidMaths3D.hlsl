static const float PI = 3.1415926;

const float K_SpikyPow2;
const float K_SpikyPow3;
const float K_SpikyPow2Grad;
const float K_SpikyPow3Grad;
const float K_Poly6;

static float LinearKernel(float dst, float radius)
{
    if (dst < radius)
    {
        return 1 - dst / radius;
    }
    return 0;
}

static float SmoothingKernelPoly6(float dst, float radius)
{
    if (dst < radius)
    {
        float v = radius * radius - dst * dst;
        return v * v * v * K_Poly6;
    }
    return 0;
}

static float SpikyKernelPow3(float dst, float radius)
{
    if (dst < radius)
    {
        float v = radius - dst;
        return v * v * v * K_SpikyPow3;
    }
    return 0;
}

static float SpikyKernelPow2(float dst, float radius)
{
    if (dst < radius)
    {
        float v = radius - dst;
        return v * v * K_SpikyPow2;
    }
    return 0;
}

static float DerivativeSpikyPow3(float dst, float radius)
{
    if (dst <= radius)
    {
        float v = radius - dst;
        return -v * v * K_SpikyPow3Grad;
    }
    return 0;
}

static float DerivativeSpikyPow2(float dst, float radius)
{
    if (dst <= radius)
    {
        float v = radius - dst;
        return -v * K_SpikyPow2Grad;
    }
    return 0;
}

float DensityKernel(float dst, float radius)
{
    return SpikyKernelPow2(dst, radius);
}

float NearDensityKernel(float dst, float radius)
{
    return SpikyKernelPow3(dst, radius);
}

float DensityDerivative(float dst, float radius)
{
    return DerivativeSpikyPow2(dst, radius);
}

float NearDensityDerivative(float dst, float radius)
{
    return DerivativeSpikyPow3(dst, radius);
}