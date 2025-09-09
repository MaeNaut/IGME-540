
cbuffer externalData : register(b0)
{
    float redOffset;
    float greenOffset;
    float blueOffset;
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

Texture2D Pixels            : register(t0);
SamplerState ClampSampler   : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    // From center of window
    float2 dir = input.uv - float2(0.5f, 0.5f);

    // Sample each channel with its own offset
    float4 fragColor;
    fragColor.r = Pixels.Sample(ClampSampler, input.uv + dir * redOffset).r;
    fragColor.g = Pixels.Sample(ClampSampler, input.uv + dir * greenOffset).g;
    fragColor.b = Pixels.Sample(ClampSampler, input.uv + dir * blueOffset).b;
    fragColor.a = 1.0f;

    return fragColor;
}
