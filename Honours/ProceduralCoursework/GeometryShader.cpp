// texture shader.cpp
#include "GeometryShader.h"


GeometryShader::GeometryShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/Terrain_vs.hlsl", L"shaders/Terrain_ps.hlsl", L"shaders/Terrain_gs.hlsl");
}


GeometryShader::~GeometryShader()
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

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the light constant buffer.
	if (LightBuffer)
	{
		LightBuffer->Release();
		LightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void GeometryShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC variableBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC IsoLevelBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;
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


	variableBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	variableBufferDesc.ByteWidth = sizeof(VariableBufferType);
	variableBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	variableBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	variableBufferDesc.MiscFlags = 0;
	variableBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&variableBufferDesc, NULL, &VariableBuffer);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&lightBufferDesc, NULL, &LightBuffer);

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&lightBufferDesc2, NULL, &LightBuffer2);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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

	// Required a CLAMPED sampler for sampling the depth map
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	// Create the texture sampler state.
	m_device->CreateSamplerState(&samplerDesc, &SampleStateClamp);


}

void GeometryShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename, WCHAR* gsFilename)
{
	InitShader(vsFilename, psFilename);

	loadGeometryShader(gsFilename);
}


void GeometryShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* DepthTexture, int VertexPositions[16], XMFLOAT3 PositionOffset, Light *light)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	VariableBufferType *VariablePtr;
	LightBufferType* lightPtr;
	LightBufferType2* lightPtr2;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj, tLightViewMatrix, tLightProjectionMatrix;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	tLightViewMatrix = XMMatrixTranspose(light->GetViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(light->GetProjectionMatrix());

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->World = tworld;// worldMatrix;
	dataPtr->View = tview;
	dataPtr->Projection = tproj;
	dataPtr->LightView = tLightViewMatrix;
	dataPtr->LightProj = tLightProjectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(MatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &MatrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(VariableBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	VariablePtr = (VariableBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	VariablePtr->Triangle1 = XMINT3(VertexPositions[0], VertexPositions[1], VertexPositions[2]);
	VariablePtr->Padding1 = 0.0f;
	VariablePtr->Triangle2 = XMINT3(VertexPositions[3], VertexPositions[4], VertexPositions[5]);
	VariablePtr->Padding2 = 0.0f;
	VariablePtr->Triangle3 = XMINT3(VertexPositions[6], VertexPositions[7], VertexPositions[8]);
	VariablePtr->Padding3 = 0.0f;
	VariablePtr->Triangle4 = XMINT3(VertexPositions[9], VertexPositions[10], VertexPositions[11]);
	VariablePtr->Padding4 = 0.0f;
	VariablePtr->Triangle5 = XMINT3(VertexPositions[12], VertexPositions[13], VertexPositions[14]);
	VariablePtr->Padding5 = 0.0f;
	VariablePtr->Offset = PositionOffset;
	VariablePtr->Padding6 = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(VariableBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &MatrixBuffer);

	bufferNumber = 1;

	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &VariableBuffer);


	deviceContext->Map(LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	lightPtr->Ambient = light->GetAmbientColour();
	lightPtr->Diffuse = light->GetDiffuseColour();

	deviceContext->Unmap(LightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &LightBuffer);

	// Send light data to vertex shader
	deviceContext->Map(LightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr2 = (LightBufferType2*)mappedResource.pData;
	lightPtr2->Position = light->GetPosition();
	lightPtr2->Padding = 1.0f;
	deviceContext->Unmap(LightBuffer2, 0);
	bufferNumber = 2;
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &LightBuffer2);

	//deviceContext->GSSetShaderResources(0, 1, &texture);

	deviceContext->PSSetShaderResources(0, 1, &texture2);

	deviceContext->PSSetShaderResources(1, 1, &texture);

	deviceContext->PSSetShaderResources(2, 1, &DepthTexture);
}

void GeometryShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &SampleState);
	deviceContext->PSSetSamplers(1, 1, &SampleStateClamp);


	// Base render function.
	BaseShader::Render(deviceContext, indexCount);

	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	deviceContext->PSSetShaderResources(0, 1, ppSRVnullptr);


	deviceContext->PSSetShaderResources(1, 1, ppSRVnullptr);


	deviceContext->PSSetShaderResources(2, 1, ppSRVnullptr);
}

