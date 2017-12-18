#include "Terrain.h"

Terrain::Terrain(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, HWND hwnd)
{
	IsoLevel = 0.001f;
	NumberOfTriangles = 0;
	TerrainGeometryShader = new GeometryShader(Device, hwnd);
	TerrainDepthShader = new DepthShader(Device, hwnd);
	ReUsePointMesh = new PointMesh(Device, DeviceContext, textureFilename, XMFLOAT3(0.0f, 0.0f, 0.0f));
	//BuildTerrain();
}


void Terrain::BuildTerrain(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, int ShapeNumber)
{
	int Counter = 0;

	//terrain only works if number of blocks is a cubic number
	//the cubic root of NUMBEROFBLOCKS defines the diameter of the cube of blocks.
	int TerrainDiameter = cbrt(NUMBEROFBLOCKS) * 3;

	int TerrainRadius = TerrainDiameter / 2;

	//for every position to build a block
	for (int x = -TerrainRadius; x < TerrainRadius; x += 3)
	{
		for (int y = -TerrainRadius; y < TerrainRadius; y += 3)
		{
			for (int z = -TerrainRadius; z < TerrainRadius; z += 3)
			{
				//build a block
				XMFLOAT3 Temp = XMFLOAT3(x, y, z);
				Blocks[Counter].Initialize(Temp);
				NumberOfTriangles += Blocks[Counter].BuildBlock(IsoLevel, Device, DeviceContext, textureFilename, LookUps, PerlinNoise, ReUsePointMesh, ShapeNumber);
				Counter++;
			}
		}
	}
}

void Terrain::Render(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix,  ID3D11ShaderResourceView* texture2, RenderTexture *DepthTexture, Camera *Cam, Light *light)
{
	for (int i = 0; i < NUMBEROFBLOCKS; i++)
	{
		Blocks[i].Render(Device, DeviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, TerrainGeometryShader, texture2, DepthTexture,  LookUps, Cam, Planes, ReUsePointMesh, light);
	}
}

void Terrain::DepthRender(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix)
{
	for (int i = 0; i < NUMBEROFBLOCKS; i++)
	{
		Blocks[i].DepthRender(Device, DeviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, TerrainDepthShader, LookUps, Planes, ReUsePointMesh);
	}
}

void Terrain::Frame(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix)
{
	ConstructFrustrum(screenDepth, projectionMatrix, viewMatrix);
}

void Terrain::ConstructFrustrum(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;

	XMFLOAT4X4 ProjectionStorage;
	XMStoreFloat4x4(&ProjectionStorage, projectionMatrix);

	XMFLOAT4 PlanesStorage[6];
	for (int i = 0; i < 6; i++)
	{
		XMStoreFloat4(&PlanesStorage[i], Planes[i]);
	}
	

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -ProjectionStorage._43 / ProjectionStorage._33;
	r = screenDepth / (screenDepth - zMinimum);
	ProjectionStorage._33 = r;
	ProjectionStorage._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	XMFLOAT4X4 MatrixStorage;
	XMStoreFloat4x4(&MatrixStorage, matrix);

	// Calculate near plane of frustum.
	PlanesStorage[0].x = MatrixStorage._14 + MatrixStorage._13;
	PlanesStorage[0].y = MatrixStorage._24 + MatrixStorage._23;
	PlanesStorage[0].z = MatrixStorage._34 + MatrixStorage._33;
	PlanesStorage[0].w = MatrixStorage._44 + MatrixStorage._43;
	

	// Calculate far plane of frustum.
	PlanesStorage[1].x = MatrixStorage._14 - MatrixStorage._13;
	PlanesStorage[1].y = MatrixStorage._24 - MatrixStorage._23;
	PlanesStorage[1].z = MatrixStorage._34 - MatrixStorage._33;
	PlanesStorage[1].w = MatrixStorage._44 - MatrixStorage._43;

	// Calculate left plane of frustum.
	PlanesStorage[2].x = MatrixStorage._14 + MatrixStorage._11;
	PlanesStorage[2].y = MatrixStorage._24 + MatrixStorage._21;
	PlanesStorage[2].z = MatrixStorage._34 + MatrixStorage._31;
	PlanesStorage[2].w = MatrixStorage._44 + MatrixStorage._41;

	// Calculate right plane of frustum.
	PlanesStorage[3].x = MatrixStorage._14 - MatrixStorage._11;
	PlanesStorage[3].y = MatrixStorage._24 - MatrixStorage._21;
	PlanesStorage[3].z = MatrixStorage._34 - MatrixStorage._31;
	PlanesStorage[3].w = MatrixStorage._44 - MatrixStorage._41;

	// Calculate top plane of frustum.
	PlanesStorage[4].x = MatrixStorage._14 - MatrixStorage._12;
	PlanesStorage[4].y = MatrixStorage._24 - MatrixStorage._22;
	PlanesStorage[4].z = MatrixStorage._34 - MatrixStorage._32;
	PlanesStorage[4].w = MatrixStorage._44 - MatrixStorage._42;

	// Calculate bottom plane of frustum.
	PlanesStorage[5].x = MatrixStorage._14 + MatrixStorage._12;
	PlanesStorage[5].y = MatrixStorage._24 + MatrixStorage._22;
	PlanesStorage[5].z = MatrixStorage._34 + MatrixStorage._32;
	PlanesStorage[5].w = MatrixStorage._44 + MatrixStorage._42;


	for (int i = 0; i < 6; i++)
	{
		Planes[i] = XMLoadFloat4(&PlanesStorage[i]);
		XMPlaneNormalize(Planes[0]);
	}
	return;
}