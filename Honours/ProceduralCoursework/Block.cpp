#include "Block.h"


void Block::Initialize(XMFLOAT3 AssignedPosition)
{
	Position = AssignedPosition;
	int Counter = 0;

	//iterate through all voxels
	for (int y = 0; y < NumVoxels; y++)//height
	{
		for (int x = 0; x < NumVoxels; x++)
		{
			for (int z = 0; z < NumVoxels; z++)
			{
				//initialize current voxel
				Voxels[x][y][z].position = XMFLOAT3(Position.x + ((float)x*3/ (float)NumVoxels), Position.y + ((float)y*3 / (float)NumVoxels), Position.z + ((float)z*3 / (float)NumVoxels));
				Voxels[x][y][z].InitializeCorners((float)NumVoxels);
				Counter++;
			}
		}
	}

	
}

float Block::BuildBlock(float IsoLevel, ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, WCHAR* textureFilename, LookUpTable LookUps, Noise PerlinNoise, PointMesh *VoxelPointMesh, int ShapeNumber)
{
	float SphereRadius = 1.5f;

	//iterate through all voxels
	for (int y = 0; y < NumVoxels; y++)//height
	{
		for (int x = 0; x < NumVoxels; x++)
		{
			for (int z = 0; z < NumVoxels; z++)
			{
				//initialize Case and render values - done here instead of in initialize so that they can be
				//reset when the shape is changed
				Voxels[x][y][z].Case = 0;
				Voxels[x][y][z].Render = false;

				for (int j = 0; j < 8; j++)
				{
					//for each corner of each voxel
					//calculate the isolevel depending on the shape
					
					if (ShapeNumber == 0)
					{
						IsoLevel = PerlinNoise.raw_noise_3d(Voxels[x][y][z].Corners[j].x, Voxels[x][y][z].Corners[j].y, Voxels[x][y][z].Corners[j].z);
					}
					else if (ShapeNumber == 1)
					{
						IsoLevel = PerlinNoise.octave_noise_3d(3, 0.4f, 1.0f, Voxels[x][y][z].Corners[j].x, Voxels[x][y][z].Corners[j].y, Voxels[x][y][z].Corners[j].z);
					}
					else if (ShapeNumber == 2)
					{
						float AmplitudeMultiplier = 5.0f;
						float FrequencyMultiplier = 10.0f;
						IsoLevel = sinf(FrequencyMultiplier*Voxels[x][y][z].Corners[j].x)/ AmplitudeMultiplier + sinf(FrequencyMultiplier*Voxels[x][y][z].Corners[j].z)/ AmplitudeMultiplier;
					}
					else if (ShapeNumber == 3)
					{
						float NewIso = sqrt(pow(SphereRadius, 2) - pow(Voxels[x][y][z].Corners[j].z, 2) - pow(Voxels[x][y][z].Corners[j].x, 2));
						IsoLevel = NULL;

						if (NewIso == NewIso)
						{
							if (Voxels[x][y][z].Corners[j].y >= 0.0f)
								IsoLevel = NewIso;
							else
							{

								IsoLevel = -NewIso;
							}
						}
					}

					//check if iso level is inside voxel
					if (Voxels[x][y][z].Corners[j].y < IsoLevel)
					{
						//Case determines which value to sample from the look up table.
						Voxels[x][y][z].Case += LookUps.Multiplier[j];
					}
				}

				//set render to true if iso-level is neither entirely above nor entirely below the voxel
				if (ShapeNumber == 0 || ShapeNumber == 1 || ShapeNumber == 2)
				{
					if (Voxels[x][y][z].Case != 0 && Voxels[x][y][z].Case != 255)
					{
						Voxels[x][y][z].Render = true;
					}
				}
				else if (ShapeNumber == 3)
				{
					//Setting render to true is slightly different for spheres as we have to check if the voxel is within the radius of the sphere.
					float Offset = 1 / 15.0f;
					float length = sqrt(pow((Voxels[x][y][z].position.x + Offset / 2), 2) + pow((Voxels[x][y][z].position.y + Offset / 2), 2) + pow((Voxels[x][y][z].position.z + Offset / 2), 2));

					if (Voxels[x][y][z].Case != 0 && Voxels[x][y][z].Case != 255 && length < SphereRadius + Offset)
					{
						Voxels[x][y][z].Render = true;
					}
				}
			}
		}
	}

	return 0.0f;
}

void Block::Render(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix, GeometryShader *TerrainGeometryShader, ID3D11ShaderResourceView* Texture2, RenderTexture *DepthTexture, LookUpTable LookUps, Camera *Cam, XMVECTOR Planes[6], PointMesh *VoxelPointMesh, Light *light)
{
	int CaseToPassIn[16];
	//Scene Render

	//iterate through all voxels
	for (int y = 0; y < NumVoxels; y++)//height
	{
		for (int x = 0; x < NumVoxels; x++)
		{
			for (int z = 0; z < NumVoxels; z++)
			{
	
				//check if this voxel should be rendered
				if (Voxels[x][y][z].Render) 
				{
					//check if this voxel is within the view frustum
					if (CheckPoint(Voxels[x][y][z].position.x, Voxels[x][y][z].position.y, Voxels[x][y][z].position.z, Planes))
					{
						//assign a new variable to contain the integer equivalent of the voxel's triangles' points.
						for (int j = 0; j < 16; j++)
						{
							CaseToPassIn[j] = LookUps.MarchingCubesLookUp[Voxels[x][y][z].Case][j];
						}
	
						VoxelPointMesh->SendData(DeviceContext);
						//// Set shader parameters (matrices and texture)
						TerrainGeometryShader->SetShaderParameters(DeviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, VoxelPointMesh->GetTexture(), Texture2, DepthTexture->GetShaderResourceView() , CaseToPassIn, Voxels[x][y][z].position, light);
						//// Render object (combination of mesh geometry and shader process
						TerrainGeometryShader->Render(DeviceContext, VoxelPointMesh->GetIndexCount());
					}
				}
			}
		}
	}

}

void Block::DepthRender(ID3D11Device *Device, ID3D11DeviceContext *DeviceContext, XMMATRIX &WorldMatrix, XMMATRIX &ViewMatrix, XMMATRIX &ProjectionMatrix, DepthShader *TerrainDepthShader, LookUpTable LookUps, XMVECTOR Planes[6], PointMesh *VoxelPointMesh)
{
	int CaseToPassIn[16];
	//Depth Render
	//iterate through all voxels
	for (int y = 0; y < NumVoxels; y++)//height
	{
		for (int x = 0; x < NumVoxels; x++)
		{
			for (int z = 0; z < NumVoxels; z++)
			{
	
				//check if this voxel should be rendered
				if (Voxels[x][y][z].Render)
				{
					//check if this voxel is within the view frustum
					if (CheckPoint(Voxels[x][y][z].position.x, Voxels[x][y][z].position.y, Voxels[x][y][z].position.z, Planes))
					{
						//assign a new variable to contain the integer equivalent of the voxel's triangles' points.
						for (int j = 0; j < 16; j++)
						{
							CaseToPassIn[j] = LookUps.MarchingCubesLookUp[Voxels[x][y][z].Case][j];
						}
	
						VoxelPointMesh->SendData(DeviceContext);
						//// Set shader parameters (matrices and texture)
						TerrainDepthShader->SetShaderParameters(DeviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, CaseToPassIn, Voxels[x][y][z].position, VoxelPointMesh->GetTexture());
						//// Render object (combination of mesh geometry and shader process
						TerrainDepthShader->Render(DeviceContext, VoxelPointMesh->GetIndexCount());



					}
				}
			}
		}
	}
}

bool Block::CheckPoint(float x, float y, float z, XMVECTOR Planes[6])
{
	// Check if the point is inside all six planes of the view frustum.
	for (int i = 0; i < 6; i++)
	{
		XMVECTOR PointVector = XMVectorSet(x, y, z, 1.0f);
		XMFLOAT4 PointVectorFloat;

		XMStoreFloat4(&PointVectorFloat, XMPlaneDotCoord(Planes[i], PointVector));
		if (PointVectorFloat.x < 0.0f)
		{
			return false;
		}
	}

	return true;
}


