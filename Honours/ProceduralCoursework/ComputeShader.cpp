// texture shader.cpp
#include "ComputeShader.h"


ComputeShader::ComputeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename) : BaseShader(device, hwnd)
{
	InitShader(filename);
}


ComputeShader::~ComputeShader()
{

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void ComputeShader::InitShader(WCHAR* csFilename, WCHAR*other)
{

}

void ComputeShader::InitShader(WCHAR* csFilename)
{
	// Load (+ compile) shader files
	LoadComputeShader(csFilename);


}


void ComputeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* in, ID3D11UnorderedAccessView* out, int horzThreads, int vertThreads)
{
	//ID3D11RenderTargetView* pNullRTV = NULL;
	//deviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);

	deviceContext->CSSetShader(m_computeShader, 0, 0);

	deviceContext->CSSetShaderResources(0, 1, &in);

	deviceContext->CSSetUnorderedAccessViews(0, 1, &out, NULL);

	deviceContext->Dispatch(horzThreads, vertThreads, 1);


	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	deviceContext->CSSetShaderResources(0, 1, ppSRVnullptr);

	ID3D11UnorderedAccessView* ppUAVnullptr[4] = { nullptr, nullptr, nullptr, nullptr };
	deviceContext->CSSetUnorderedAccessViews(0, 4, ppUAVnullptr, nullptr);

	deviceContext->CSSetShader(NULL, 0, 0);


}


void ComputeShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &SampleState);

	
	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



