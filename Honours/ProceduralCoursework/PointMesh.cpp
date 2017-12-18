// Mesh.cpp
#include "pointmesh.h"
#include <ctime>

PointMesh::PointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, XMFLOAT3 Position)
{
	VertexCount = 1;

	InitBuffers(device, Position);

	// Load the texture for this model.
	LoadTexture(device, deviceContext, textureFilename);
}


PointMesh::~PointMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void PointMesh::InitBuffers(ID3D11Device* device)
{
}

void PointMesh::InitBuffers(ID3D11Device* device, XMFLOAT3 Position)
{
	unsigned long* indices;

	// Set the number of vertices in the vertex array.

	// Set the number of indices in the index array.
	m_indexCount = VertexCount;

	// Create the vertex array.
	Vertices = new VertexType[VertexCount];
	

	// Create the index array.
	indices = new unsigned long[m_indexCount];


	Vertices[0].position = Position;

	for (int i = 0; i < VertexCount; i++)
	{
		Vertices[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	}

	// Load the index array with data.
	for (int i = 0; i < VertexCount; i++)
	{
		indices[i] = i;
	}

					 // Set up the description of the static vertex buffer.
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth = sizeof(VertexType)* VertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	VertexData.pSysMem = Vertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	device->CreateBuffer(&VertexBufferDesc, &VertexData, &m_vertexBuffer);

	// Set up the description of the static index buffer.
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	IndexData.pSysMem = indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&IndexBufferDesc, &IndexData, &m_indexBuffer);

	delete[] indices;
	indices = 0;
}

void PointMesh::SendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

