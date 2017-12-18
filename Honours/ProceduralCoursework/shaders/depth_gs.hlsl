cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer VariableBuffer : register(cb1)
{
	int3 Triangle1;
	float Padding1;
	int3 Triangle2;
	float Padding2;
	int3 Triangle3;
	float Padding3;
	int3 Triangle4;
	float Padding4;
	int3 Triangle5;
	float Padding5;
	float3 Offset;
	float Padding6;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// pixel input type
struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

float3 MCIntToFloat3(int integer, float3 position, float Diameter)
{
	switch (integer)
	{
	case 0:
		return float3(position.x, position.y + (Diameter / 2), position.z);
		break;
	case 1:
		return float3(position.x + (Diameter / 2), position.y + Diameter, position.z);
		break;
	case 2:
		return float3(position.x + Diameter, position.y + (Diameter / 2), position.z);
		break;
	case 3:
		return float3(position.x + (Diameter / 2), position.y, position.z);
		break;
	case 4:
		return float3(position.x, position.y + (Diameter / 2), position.z + Diameter);
		break;
	case 5:
		return float3(position.x + (Diameter / 2), position.y + Diameter, position.z + Diameter);
		break;
	case 6:
		return float3(position.x + Diameter, position.y + (Diameter / 2), position.z + Diameter);
		break;
	case 7:
		return float3(position.x + (Diameter / 2), position.y, position.z + Diameter);
		break;
	case 8:
		return float3(position.x, position.y, position.z + (Diameter / 2));
		break;
	case 9:
		return float3(position.x, position.y + Diameter, position.z + (Diameter / 2));
		break;
	case 10:
		return float3(position.x + Diameter, position.y + Diameter, position.z + (Diameter / 2));
		break;
	case 11:
		return float3(position.x + Diameter, position.y, position.z + (Diameter / 2));
		break;
	default:
		return float3(0.0, 0.0, 0.0);
		break;
	}
}

[maxvertexcount(15)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input[0].position.w = 1.0f;

	int VertexPositions[15] = { Triangle1.x, Triangle1.y, Triangle1.z, Triangle2.x, Triangle2.y, Triangle2.z, Triangle3.x, Triangle3.y, Triangle3.z , Triangle4.x, Triangle4.y, Triangle4.z , Triangle5.x, Triangle5.y, Triangle5.z };

	//for each triangle
	for (int i = 0; i < 15; i+=3)
	{
		//for each vertex in triangle
		if (VertexPositions[i] != -1)
		{
			float3 vposition[3];
			for (int j = 0; j < 3; j++)
			{
				vposition[j] = MCIntToFloat3(VertexPositions[i + j], input[0].position + Offset, (3.0f/15.0f));
			}

			for (int j = 0; j < 3; j++)
			{

				vposition[j] = mul(vposition[j], (float3x3) worldMatrix) + input[0].position;
				output.position = mul(float4(vposition[j], 1.0), viewMatrix);
				output.position = mul(output.position, projectionMatrix);

				output.depthPosition = output.position;

				triStream.Append(output);
			}
			triStream.RestartStrip();
		}
	}
}
