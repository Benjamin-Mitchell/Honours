// Colour shader.h
// Simple shader example.
#ifndef _DEPTHSHADER_H_
#define _DEPTHSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{

public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, int VertexPositions[16], XMFLOAT3 PositionOffset,  ID3D11ShaderResourceView* texture);

private:

	struct VariableBufferType
	{
		XMINT3 Triangle1;
		float Padding1;
		XMINT3 Triangle2;
		float Padding2;
		XMINT3 Triangle3;
		float Padding3;
		XMINT3 Triangle4;
		float Padding4;
		XMINT3 Triangle5;
		float Padding5;
		XMFLOAT3 Offset;
		float Padding6;
	};

	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* MatrixBuffer, *VariableBuffer;
};

#endif