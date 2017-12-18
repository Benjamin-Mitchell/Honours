// depth shader.cpp
#include "depthshader.h"


DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/depth_vs.hlsl", L"shaders/depth_ps.hlsl");
	loadGeometryShader(L"shaders/depth_gs.hlsl");
}


DepthShader::~DepthShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
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


void DepthShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC variableBufferDesc;

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
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	variableBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	variableBufferDesc.ByteWidth = sizeof(VariableBufferType);
	variableBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	variableBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	variableBufferDesc.MiscFlags = 0;
	variableBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&variableBufferDesc, NULL, &VariableBuffer);

}


void DepthShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, int VertexPositions[16], XMFLOAT3 PositionOffset, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	VariableBufferType *VariablePtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	bufferNumber = 0;

	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

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

	bufferNumber = 1;

	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &VariableBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->PSSetShaderResources(1, 1, &texture);
}



