// texture shader.h
#ifndef _POSTPROCESSSHADER_H_
#define _POSTPROCESSSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class PostProcessShader : public BaseShader
{

public:

	PostProcessShader(ID3D11Device* device, HWND hwnd);
	~PostProcessShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* Depthtexture, ID3D11ShaderResourceView* Scenetexture, float ScreenWidth, float ScreenHeight);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:

	struct ScreenBufferType
	{
		float ScreenWidth;
		float ScreenHeight;
		XMFLOAT2 Padding;
	};

	ID3D11Buffer* MatrixBuffer, *ScreenBuffer;
	ID3D11SamplerState* SampleState;
};

#endif