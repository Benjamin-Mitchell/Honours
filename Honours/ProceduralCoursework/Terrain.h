#ifndef _TERRAIN_H
#define _TERRAIN_H

#define NUMBEROFBLOCKS 64


#include "Block.h"

class Terrain
{
public:

	Terrain(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, HWND hwnd);

	void BuildTerrain(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, int ShapeNumber);
	void Render(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix, ID3D11ShaderResourceView* texture2, RenderTexture *DepthTexture, Camera *Cam, Light *light);
	void DepthRender(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix);
	void Frame(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix);

private:

	void ConstructFrustrum(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix);

	float IsoLevel;

	Block Blocks[NUMBEROFBLOCKS];

	GeometryShader *TerrainGeometryShader;
	DepthShader *TerrainDepthShader;
	
	int NumberOfTriangles;

	LookUpTable LookUps;

	XMVECTOR Planes[6];

	Noise PerlinNoise;

	PointMesh *ReUsePointMesh;

};

#endif