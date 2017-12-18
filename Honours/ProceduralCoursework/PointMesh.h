// Tessellation Mesh, point list for tessellation
#ifndef _POINTMESH_H_
#define _POINTMESH_H_

#include "../DXFramework/BaseMesh.h"
#include <vector>
#include "LookUpTables.h"

using namespace DirectX;

class PointMesh : public BaseMesh
{

public:

	PointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, XMFLOAT3 Position);
	~PointMesh();

	VertexType *Vertices;

	void SendData(ID3D11DeviceContext*);

	int VertexCount;

protected:

	D3D11_BUFFER_DESC VertexBufferDesc, IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA VertexData, IndexData;

	void InitBuffers(ID3D11Device* device);
	void InitBuffers(ID3D11Device* device, XMFLOAT3 Position);


};

#endif
