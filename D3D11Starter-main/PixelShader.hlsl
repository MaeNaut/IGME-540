#include "ShaderIncludes.hlsli"

#define MAX_LIGHTS 128

Texture2D SurfaceTexture    : register(t0);
Texture2D NormalMap         : register(t1);
SamplerState BasicSampler   : register(s0);

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
        
        // Cut the specular if the diffuse contribution is zero
        // - any() returns 1 if any component of the param is non-zero
        // - In other words:
        // - If the diffuse amount is 0, any(diffuse) returns 0
        // - If the diffuse amount is != 0, any(diffuse) returns 1
        // - So when diffuse is 0, specular becomes 0
        specTerm *= any(diffuseTerm);

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
        
        // Cut the specular if the diffuse contribution is zero
        // - any() returns 1 if any component of the param is non-zero
        // - In other words:
        // - If the diffuse amount is 0, any(diffuse) returns 0
        // - If the diffuse amount is != 0, any(diffuse) returns 1
        // - So when diffuse is 0, specular becomes 0
        specTerm *= any(diffuseTerm);
        
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
