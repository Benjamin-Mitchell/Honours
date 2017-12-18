// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry


Texture2D DepthTexture : register(t0);
Texture2D SceneTexture : register(t1);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
	float4 DepthColor;
	float4 OutlineColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float2 TextCoords[8] = {input.texCoord1 ,input.texCoord2, input.texCoord3, input.texCoord4, input.texCoord5, input.texCoord6, input.texCoord7, input.texCoord8};
	
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = SceneTexture.Sample(Sampler0, input.tex);
	DepthColor = DepthTexture.Sample(Sampler0, input.tex);
	
	for (int i = 0; i < 8; i++)
	{
		float4 tempColor = DepthTexture.Sample(Sampler0, TextCoords[i]);
		float Threshold = 0.0005f;
		//change to comparison if near
		if (abs(DepthColor.x - tempColor.x) > Threshold || abs(DepthColor.y - tempColor.y) > Threshold || abs(DepthColor.z - tempColor.z) > Threshold || abs(DepthColor.w - tempColor.w) > Threshold)
		{
			return OutlineColor;
		}
	}

    return textureColor;
}