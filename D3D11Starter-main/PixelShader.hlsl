#include "ShaderIncludes.hlsli"

#define MAX_LIGHTS 128

Texture2D Albedo                        : register(t0);
Texture2D NormalMap                     : register(t1);
Texture2D RoughnessMap                  : register(t2);
Texture2D MetalnessMap                  : register(t3);
Texture2D ShadowMap                     : register(t4);
SamplerState BasicSampler               : register(s0);
SamplerComparisonState ShadowSampler    : register(s1);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
    float2 scale;
    float2 offset;
    
    float roughness;
    float3 cameraPosition;
    
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
float3 DirectionalLight(float3 normal, Light light, float3 worldPosition, float3 surfaceColor, float roughness, float3 specularColor, float metalness)
{
    float3 lightDirection = normalize(light.Direction);
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    // Calculate the light amounts
    float diff = DiffusePBR(normal, -lightDirection);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, -lightDirection, viewVector, roughness, specularColor, F);
    
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    // Combine the final diffuse and specular values for this light
    float3 returnLight = (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
    
    // Return the resulting directional light
    return returnLight;
}

// Calculate point light
float3 PointLight(float3 normal, Light light, float3 worldPosition, float3 surfaceColor, float roughness, float3 specularColor, float metalness)
{
    float3 lightDirection = normalize(worldPosition - light.Position);
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    // Calculate the light amounts
    float diff = DiffusePBR(normal, -lightDirection);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, -lightDirection, viewVector, roughness, specularColor, F);
    
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    // Combine the final diffuse and specular values for this light
    float3 returnLight = (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
    
    // Return the resulting directional light
    return returnLight * Attenuate(light, worldPosition);

}

// Calculate spot light
float3 SpotLight(float3 normal, Light light, float3 worldPosition, float3 surfaceColor, float roughness, float3 specularColor, float metalness)
{
    // Get cos(angle) between pixel and light direction
    float pixelAngle = saturate(dot(normalize(worldPosition - light.Position), normalize(light.Direction)));
    
    // Get cosines of angles and calculate range
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    // Linear falloff over the range, clamp 0-1, apply to light calc
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return PointLight(normal, light, worldPosition, surfaceColor, roughness, specularColor, metalness) * spotTerm;
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
    // Clean up un-normalized normals and tangents
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    
    // Calculate texture coordinates
    input.uv = input.uv * scale + offset;
    
    
    // Sample and unpack normal
    float3 unpackedNormal = (NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1);
    unpackedNormal = (unpackedNormal); // Don’t forget to normalize!

    // Feel free to adjust/simplify this code to fit with your existing shader(s)
    // Simplifications include not re-normalizing the same vector more than once!
    float3 N = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    // Assumes that input.normal is the normal later in the shader
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!
    

    // Texture
    float3 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    surfaceColor *= colorTint.rgb;
    
    // Roughness & Metalness
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    //float roughness = 0.3f;
    //float metalness = 1.0f;
    
    // Specular color
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);

	// Total light
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);

    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
        ShadowSampler,
        shadowUV,
        distToLight).r;
    
    // Sort each light by their type, calculate accordingly, and add to the total light
    for (int i = 0; i < lightCount; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                float3 lightResult = DirectionalLight(input.normal, lights[i], input.worldPosition, surfaceColor, roughness, specularColor, metalness);
            
                // If this is the first light, apply the shadowing result
                if (i == 0)
                {
                    lightResult *= shadowAmount;
                }
            
                // Add this light's result to the total light for this pixel
                totalLight += lightResult;
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(input.normal, lights[i], input.worldPosition, surfaceColor, roughness, specularColor, metalness);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(input.normal, lights[i], input.worldPosition, surfaceColor, roughness, specularColor, metalness);
                break;
        }
    }
    
    
    return float4(pow(totalLight, 1.0 / 2.2f), 1);
}
