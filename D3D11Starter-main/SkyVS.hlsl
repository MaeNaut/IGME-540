#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

SkyVertexToPixel main( VertexShaderInput input )
{
    SkyVertexToPixel output;
    
    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    matrix vntp = mul(projection, viewNoTranslation);
    output.position = mul(vntp, float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;

	return output;
}