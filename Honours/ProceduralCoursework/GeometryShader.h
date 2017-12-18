// texture shader.h
#ifndef _GeometryShader_H_
#define _GeometryShader_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
{

public:

	GeometryShader(ID3D11Device* device, HWND hwnd);
	~GeometryShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* DepthTexture, int VertexPositions[16], XMFLOAT3 PositionOffset, Light* light);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:

	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
		XMMATRIX LightView;
		XMMATRIX LightProj;
	};

	struct LightBufferType
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
	};

	struct LightBufferType2
	{
		XMFLOAT3 Position;
		float Padding;
	};

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
	void InitShader(WCHAR*, WCHAR*, WCHAR*);

private:
	ID3D11Buffer* MatrixBuffer, *VariableBuffer, *LightBuffer, *LightBuffer2;
	ID3D11SamplerState* SampleState, *SampleStateClamp;
};

#endif#pragma once
