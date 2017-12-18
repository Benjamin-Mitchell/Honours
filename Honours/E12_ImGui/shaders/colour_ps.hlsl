// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

cbuffer ColourBuffer : register(cb0)
{
	float4 colour;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 outcolour = float4(colour.x/255.f, colour.y / 255.f, colour.z / 255.f, 1.0);

	return colour;
}