// texture shader.cpp
#include "PostProcessShader.h"


PostProcessShader::PostProcessShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/PostProcess_vs.hlsl", L"shaders/PostProcess_ps.hlsl");
}


PostProcessShader::~PostProcessShader()
{
	// Release the sampler state.
	if (SampleState)
	{
		SampleState->Release();
		SampleState = 0;
	}

	// Release the matrix constant buffer.
	if (MatrixBuffer)
	{
		MatrixBuffer->Release();
		MatrixBuffer = 0;
	}

	if (ScreenBuffer)
	{
		ScreenBuffer->Release();
		ScreenBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void PostProcessShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC screenBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &MatrixBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	screenBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	screenBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenBufferDesc.MiscFlags = 0;
	screenBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&screenBufferDesc, NULL, &ScreenBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	m_device->CreateSamplerState(&samplerDesc, &SampleState);

}


void PostProcessShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* Depthtexture, ID3D11ShaderResourceView* Scenetexture, float ScreenWidth, float ScreenHeight)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	ScreenBufferType* screenPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	deviceContext->Unmap(MatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &MatrixBuffer);


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(ScreenBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	screenPtr = (ScreenBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	screenPtr->ScreenWidth = ScreenWidth;// worldMatrix;
	screenPtr->ScreenHeight = ScreenHeight;
	screenPtr->Padding = XMFLOAT2(0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(ScreenBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &ScreenBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &Depthtexture);

	deviceContext->PSSetShaderResources(1, 1, &Scenetexture);
}

void PostProcessShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &SampleState);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



