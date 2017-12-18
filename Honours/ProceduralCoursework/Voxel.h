#ifndef _VOXEL_H
#define _VOXEL_H

#include "D3D.h"
#include "pointmesh.h"

class Voxel
{
public:
	XMFLOAT3 position;

	/*points of the 8 corners of the voxel
	- first face, lower Z value
	- 0 = bottom left, 1 = top left, 2 = top right, 3 = bottom right
	- second face, higher Z value
	- 4 = bottom left, 5 = top left, 6 = top right, 7 = bottom right
	*/
	XMFLOAT3 Corners[8];

	int Case = 0;

	void InitializeCorners(float NumberOfVoxels);

	bool Render = false;
};




#endif