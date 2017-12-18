#ifndef _BLOCK_H
#define _BLOCK_H

#include "D3D.h"
#include "GeometryShader.h"
#include "DepthShader.h"
#include "Voxel.h"
#include "LookUpTables.h"
#include "../DXFramework/Camera.h"
#include "../DXFramework/CubeMesh.h"
#include "Noise.h"
#include "../DXFramework/RenderTexture.h"

const int NumVoxels = 15;

class Block
{
public:
	XMFLOAT3 Position;
	

	Voxel Voxels[NumVoxels][NumVoxels][NumVoxels];

	void Initialize(XMFLOAT3 AssignedPosition);

	float BuildBlock(float IsoLevel, ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, LookUpTable LookUps, Noise PerlinNoise, PointMesh *VoxelPointMesh, int ShapeNumber);

	void Render(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix, GeometryShader *TerrainGeometryShader, ID3D11ShaderResourceView* Texture2, RenderTexture *DepthTexture, LookUpTable LookUps, Camera *Cam, XMVECTOR Planes[6], PointMesh *VoxelPointMesh, Light *light);
	
	void DepthRender(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix, DepthShader *TerrainDepthShader, LookUpTable LookUps, XMVECTOR Planes[6], PointMesh *VoxelPointMesh);

private:

	bool CheckPoint(float x, float y, float z, XMVECTOR Planes[6]);
};



#endif