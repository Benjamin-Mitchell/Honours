#ifndef _TRIMESH_H_
#define _TRIMESH_H_

#include "../DXFramework/BaseMesh.h"

using namespace DirectX;

class TriangleMesh : public BaseMesh
{

public:
	TriangleMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, XMFLOAT3 VertexPositions[3]);
	~TriangleMesh();

protected:

	void InitBuffers(ID3D11Device* device);
	void InitBuffers(ID3D11Device* device, XMFLOAT3 VertexPositions[3]);

};

#endif
