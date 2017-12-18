// texture shader.h
#ifndef _COMPUTESHADER_H_
#define _COMPUTESHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class ComputeShader : public BaseShader
{

public:

	ComputeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename);
	~ComputeShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* in, ID3D11UnorderedAccessView* out, int horzThreads, int vertThreads);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*);
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* MatrixBuffer;
	ID3D11SamplerState* SampleState;
};

#endif