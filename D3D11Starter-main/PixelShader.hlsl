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


	// Calculate texture coordinates
    input.uv = input.uv * scale + offset;
    
    // Texture
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);

	// Total light
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);

	// Ambient definition
    float3 ambientColor = float3(0.25f, 0.25f, 0.25f);
    float3 ambientTerm = ambientColor * surfaceColor;

	// Light definition
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 1.0f;
    float3 lightDirection = float3(1.0f, 0.0f, 0.0f);

	// Diffuse calculation
    float3 diffuseTerm =
		max(dot(input.normal, -lightDirection), 0) *
		lightColor * lightIntensity * surfaceColor;

	// Specular calculation
    float3 refl = reflect(lightDirection, input.normal);
    float viewVector = normalize(cameraPosition - input.worldPos);

    float3 specTerm = pow(max(dot(refl, viewVector), 0), 256) *
		lightColor * lightIntensity * surfaceColor;
	  
	// Combine all lights
    totalLight += ambientTerm + diffuseTerm + specTerm;

    return float4(totalLight, 1);



	//return float4(1, 1, 1, 1) * colorTint;
	
    //return surfaceColor * colorTint;
}