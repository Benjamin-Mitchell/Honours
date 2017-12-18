#include "Voxel.h"

void Voxel::InitializeCorners(float NumberOfVoxels)
{
	//first four have lower Z values
	Corners[0] = position;
	Corners[1] = XMFLOAT3(position.x, position.y + (3.0f/ NumberOfVoxels), position.z);
	Corners[2] = XMFLOAT3(position.x + (3.0f/ NumberOfVoxels), position.y + (3.0f/ NumberOfVoxels), position.z);
	Corners[3] = XMFLOAT3(position.x + (3.0f/ NumberOfVoxels), position.y, position.z);

	//last four have higher Z values.
	Corners[4] = XMFLOAT3(position.x, position.y, position.z + (3.0f/ NumberOfVoxels));
	Corners[5] = XMFLOAT3(position.x, position.y + (3.0f/ NumberOfVoxels), position.z + (3.0f/ NumberOfVoxels));
	Corners[6] = XMFLOAT3(position.x + (3.0f/ NumberOfVoxels), position.y + (3.0f/ NumberOfVoxels), position.z + (3.0f/ NumberOfVoxels));
	Corners[7] = XMFLOAT3(position.x + (3.0f/ NumberOfVoxels), position.y, position.z + (3.0f/ NumberOfVoxels));
}