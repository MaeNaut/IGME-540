#include "ShaderIncludes.hlsli"

#define MAX_LIGHTS 128

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
    
    Light lights[MAX_LIGHTS];
    int lightCount;
}

// Attenuate the light as the object is further away
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

// Calculate directional light
float3 DirectionalLight(float3 normal, Light light, float3 worldPosition, float4 surfaceColor)
{
    float3 returnLight = { 0.0f, 0.0f, 0.0f };
    float3 lightDirection = normalize(light.Direction);
    
    // Diffuse calculation
    float3 diffuseTerm =
		max(dot(normal, -lightDirection), 0.0f) *
		light.Color * light.Intensity * surfaceColor.xyz;
    returnLight += diffuseTerm;

	// Specular calculation
    if (roughness < 1.0f)
    {
        float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
        float3 refl = reflect(lightDirection, normal);
        float3 viewVector = normalize(cameraPosition - worldPosition);
        
        float3 specTerm = pow(max(dot(refl, viewVector), 0.0f), specExponent) *
            light.Color * light.Intensity * surfaceColor.xyz;
        
        returnLight += specTerm;
    }
    
    // Return the resulting directional light
    return returnLight;
}

// Calculate point light
float3 PointLight(float3 normal, Light light, float3 worldPosition, float4 surfaceColor)
{
    float3 returnLight = { 0.0f, 0.0f, 0.0f };
    float3 lightDirection = normalize(worldPosition - light.Position);
    
    // Diffuse calculation
    float3 diffuseTerm =
		max(dot(normal, -lightDirection), 0.0f) *
		light.Color * light.Intensity * surfaceColor.xyz;
    returnLight += diffuseTerm;

	// Specular calculation
    if (roughness < 1.0f)
    {
        float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
        float3 refl = reflect(lightDirection, normal);
        float3 viewVector = normalize(cameraPosition - worldPosition);
        
        float3 specTerm = pow(max(dot(refl, viewVector), 0.0f), specExponent) *
            light.Color * light.Intensity * surfaceColor.xyz;
        
        returnLight += specTerm;
    }
    
    // Return the resulting point light with attenuation
    return returnLight * Attenuate(light, worldPosition);
}

// Calculate spot light
float3 SpotLight(float3 normal, Light light, float3 worldPosition, float4 surfaceColor)
{
    // Get cos(angle) between pixel and light direction
    float pixelAngle = saturate(dot(normalize(worldPosition - light.Position), normalize(light.Direction)));
    
    // Get cosines of angles and calculate range
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    // Linear falloff over the range, clamp 0-1, apply to light calc
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return PointLight(normal, light, worldPosition, surfaceColor) * spotTerm;
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

    // Texture
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    surfaceColor *= colorTint;

	// Total light
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);

	// Ambient definition
    float3 ambientTerm = ambient * surfaceColor.xyz;
    totalLight += ambientTerm;

    // Sort each light by their type, calculate accordingly, and add to the total light
    for (int i = 0; i < lightCount; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalLight(input.normal, lights[i], input.worldPosition, surfaceColor);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(input.normal, lights[i], input.worldPosition, surfaceColor);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(input.normal, lights[i], input.worldPosition, surfaceColor);
                break;
        }
    }

    return float4(totalLight, 1);
}
