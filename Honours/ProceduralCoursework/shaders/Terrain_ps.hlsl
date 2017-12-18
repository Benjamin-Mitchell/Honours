// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1);
Texture2D DepthMapTexture : register(t2);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float3 tex : position;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;

	bias = 0.0001f;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	
	color = ambientColor;
	
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the 	projected texture coordinate location.
		depthValue = DepthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, input.lightPos));

			if (lightIntensity > 0.0f)
			{
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
				color += (diffuseColor * lightIntensity);

				// Saturate the final light color.
				color = saturate(color);


			}
		}
	}

	float3 sampleWeightings = normalize(input.normal * input.normal);
	
	float3 blendedColor =	sampleWeightings.xxx * Texture0.Sample(SampleType, input.tex.yz) +
							sampleWeightings.yyy * Texture0.Sample(SampleType, input.tex.zx) +
							sampleWeightings.zzz * Texture1.Sample(SampleType, input.tex.xy );


	color.rgb = color.rgb * blendedColor;
	
	return color;
}


