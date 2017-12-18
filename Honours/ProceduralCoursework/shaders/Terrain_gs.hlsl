cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
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

cbuffer LightBuffer : register(cb2)
{
	float3 LightPosition;
	float Padding;
}

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
	float3 tex : position;
	float3 normal : NORMAL;
	float4 lightViewPosition: TEXCOORD1;
	float3 lightPos: TEXCOORD2;
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

//int perm[512] = {
//	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
//	8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
//	35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
//	134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
//	55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
//	18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
//	250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
//	189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
//	172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
//	228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
//	107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
//	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
//
//	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
//	8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
//	35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
//	134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
//	55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
//	18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
//	250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
//	189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
//	172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
//	228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
//	107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
//	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
//};
//
//int grad3[12][3] = {
//	{ 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
//	{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
//	{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 }
//};
//
////float dot(const int3 g, const float x, const float y) 
////{ 
////	return g.x * x + g.y * y;
////}
//
//float dot(const float x, const float y, const float z, const float x2, const float y2, const float z2)
//{ 
//	return x2 * x + y2 * y + z2 * z;
//}
//
//
//
//float raw_noise_3d(const float x, const float y, const float z) {
//	// 3D raw Simplex noise
//	float n0, n1, n2, n3; // Noise contributions from the four corners
//
//	// Skew the input space to determine which simplex cell we're in
//	float F3 = 1.0f / 3.0f;
//	float s = (x + y + z)*F3; // Very nice and simple skew factor for 3D
//	int i = floor(x + s);
//	int j = floor(y + s);
//	int k = floor(z + s);
//
//	float G3 = 1.0f / 6.0f; // Very nice and simple unskew factor, too
//	float t = (i + j + k)*G3;
//	float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
//	float Y0 = j - t;
//	float Z0 = k - t;
//	float x0 = x - X0; // The x,y,z distances from the cell origin
//	float y0 = y - Y0;
//	float z0 = z - Z0;
//
//	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
//	// Determine which simplex we are in.
//	int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
//	int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
//
//	if (x0 >= y0) {
//		if (y0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // X Y Z order
//		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
//		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
//	}
//	else { // x0<y0
//		if (y0<z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
//		else if (x0<z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
//		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
//	}
//
//	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
//	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
//	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
//	// c = 1/6.
//	float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
//	float y1 = y0 - j1 + G3;
//	float z1 = z0 - k1 + G3;
//	float x2 = x0 - i2 + 2.0f*G3; // Offsets for third corner in (x,y,z) coords
//	float y2 = y0 - j2 + 2.0f*G3;
//	float z2 = z0 - k2 + 2.0f*G3;
//	float x3 = x0 - 1.0f + 3.0f*G3; // Offsets for last corner in (x,y,z) coords
//	float y3 = y0 - 1.0f + 3.0f*G3;
//	float z3 = z0 - 1.0f + 3.0f*G3;
//
//	// Work out the hashed gradient indices of the four simplex corners
//	int ii = i & 255;
//	int jj = j & 255;
//	int kk = k & 255;
//	int gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
//	int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
//	int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
//	int gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;
//
//	// Calculate the contribution from the four corners
//	float t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
//	if (t0<0) n0 = 0.0;
//	else {
//		t0 *= t0;
//		n0 = t0 * t0 * dot(x0, y0, z0, grad3[gi0][0], grad3[gi0][1], grad3[gi0][2]);
//	}
//
//	float t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
//	if (t1<0) n1 = 0.0;
//	else {
//		t1 *= t1;
//		n1 = t1 * t1 * dot(x1, y1, z1, grad3[gi1][0], grad3[gi1][1], grad3[gi1][2]);
//	}
//
//	float t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
//	if (t2<0) n2 = 0.0;
//	else {
//		t2 *= t2;
//		n2 = t2 * t2 * dot(x2, y2, z2, grad3[gi2][0], grad3[gi2][1], grad3[gi2][2]);
//	}
//
//	float t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
//	if (t3<0) n3 = 0.0;
//	else {
//		t3 *= t3;
//		n3 = t3 * t3 * dot(x3, y3, z3, grad3[gi3][0], grad3[gi3][1], grad3[gi3][2]);
//	}
//
//	// Add contributions from each corner to get the final noise value.
//	// The result is scaled to stay just inside [-1,1]
//	return 32.0*(n0 + n1 + n2 + n3);
//}
//
//float octave_noise_3d(const float octaves, const float persistence, const float scale, const float x, const float y, const float z) {
//	float total = 0;
//	float frequency = scale;
//	float amplitude = 1;
//
//	// We have to keep track of the largest possible amplitude,
//	// because each octave adds more, and we need a value in [-1, 1].
//	float maxAmplitude = 0;
//
//	for (int i = 0; i < octaves; i++) {
//		total += raw_noise_3d(x * frequency, y * frequency, z * frequency) * amplitude;
//
//		frequency *= 2;
//		maxAmplitude += amplitude;
//		amplitude *= persistence;
//	}
//
//	return total / maxAmplitude;
//}

[maxvertexcount(15)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input[0].position.w = 1.0f;

	int VertexPositions[15] = { Triangle1.x, Triangle1.y, Triangle1.z, Triangle2.x, Triangle2.y, Triangle2.z, Triangle3.x, Triangle3.y, Triangle3.z , Triangle4.x, Triangle4.y, Triangle4.z , Triangle5.x, Triangle5.y, Triangle5.z };

	//for each triangle
	for (int i = 0; i < 15; i += 3)
	{
		//for each vertex in triangle
		if (VertexPositions[i] != -1)
		{
			float3 vposition[3];
			for (int j = 0; j < 3; j++)
			{
				vposition[j] = MCIntToFloat3(VertexPositions[i + j], input[0].position + Offset, (3.0f / 15.0f));
			}

			for (int j = 0; j < 3; j++)
			{
				output.tex = vposition[j];
				float4 WorldPosition;

				vposition[j] = mul(vposition[j], (float3x3) worldMatrix) + input[0].position;
				output.position = mul(float4(vposition[j], 1.0), viewMatrix);
				output.position = mul(output.position, projectionMatrix);

				output.lightViewPosition = mul(float4(vposition[j], 1.0), worldMatrix);
				output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
				output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

				WorldPosition = mul(vposition[j], worldMatrix);

				output.lightPos = LightPosition.xyz - WorldPosition.xyz;
				output.lightPos = normalize(output.lightPos);



				float3 U = vposition[1] - vposition[0];
				float3 V = vposition[2] - vposition[0];
				output.normal = normalize(float3((U.y*V.z) - (U.z*V.y), (U.z*V.x) - (U.x*V.z), (U.x*V.y) - (U.y*V.x)));


				triStream.Append(output);
			}
			triStream.RestartStrip();
		}
	}
}
