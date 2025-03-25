#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture  : register(t0);
SamplerState BasicSampler : register(s0);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
    float2 scale;
    float2 offset;
    float roughness;
    float3 cameraPosition;
    float3 ambient;
    Light directionalLight1;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    // Just return the input color
    // - This color (like most values passing through the rasterizer) is 
    //   interpolated for each pixel between the corresponding vertices 
    //   of the triangle we're rendering

    // Clean up un-normalized normals and tangents
    input.normal = normalize(input.normal);

	// Calculate texture coordinates
    input.uv = input.uv * scale + offset;

    //// Sample the normal map
    //float3 normalFromMap = NormalMap.Sample(BasicSampler, input.uv);

    //// Unpack the normals
    //normalFromMap = normalFromMap * 2 - 1;

    //input.normal = normalFromMap;
    
    // Texture
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    surfaceColor *= colorTint;

	// Total light
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);

	// Ambient definition
    float3 ambientTerm = ambient * surfaceColor;

	// Diffuse calculation
    float3 diffuseTerm =
		max(dot(input.normal, -directionalLight1.Direction), 0.0f) *
		directionalLight1.Color * directionalLight1.Intensity * surfaceColor;

	// Specular calculation
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    float3 refl = reflect(directionalLight1.Direction, input.normal);
    float viewVector = normalize(cameraPosition - input.worldPos);

    float3 specTerm = pow(max(dot(refl, viewVector), 0.0f), specExponent) *
        directionalLight1.Color * directionalLight1.Intensity * surfaceColor;
	  
	// Combine all lights
    totalLight += ambientTerm + diffuseTerm + specTerm;

    return float4(specTerm, 1);



	//return float4(1, 1, 1, 1) * colorTint;
	
    //return surfaceColor * colorTint;
}