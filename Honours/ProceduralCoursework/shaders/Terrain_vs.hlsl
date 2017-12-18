struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


InputType main(InputType input)
{
	// No processing required so Vertex shader passes values onto next stage.
	// You could manipulate the points in the mesh before passing them on.
	return input;
}
