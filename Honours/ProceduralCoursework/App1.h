// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/baseapplication.h"
#include "D3D.h"

#include "../DXFramework/quadmesh.h"
#include "../DXFramework/OrthoMesh.h"
#include "../DXFramework/RenderTexture.h"

#include "TextureShader.h"
#include "PostProcessShader.h"
#include "ComputeShader.h"

#include "Terrain.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	bool Render();

private:

	void GUI();
	void RenderTextures();

	//storage for screen size for ortho mesh renders.
	int ScreenWidth, ScreenHeight;

	//storage for checking if the user changed the shape number
	int ShapeNumber, PreviousShapeNumber;

	Light *m_Light;

	TextureShader *m_TextureShader;
	PostProcessShader *m_PostProcessShader;
	ComputeShader *m_HorzBlurShader, *m_VertBlurShader;

	//MC = Marching Cubes
	Terrain *MCTerrain;

	RenderTexture *m_DepthTexture, *m_PostProcessTexture, *m_CamDepthTexture, *m_FirstBlurTexture, *m_SecondBlurTexture;
	Texture *m_Texture;
	OrthoMesh *m_OrthoMesh;
};

#endif