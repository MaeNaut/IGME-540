#include "ShaderIncludes.hlsli"

TextureCube SkyboxTexture	: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main( SkyVertexToPixel input ) : SV_TARGET
{
    return SkyboxTexture.Sample(BasicSampler, input.sampleDir);
}