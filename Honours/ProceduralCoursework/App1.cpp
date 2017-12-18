// Lab1.cpp
// Lab 1 example, simple textured quad
#include "App1.h"

App1::App1()
{
	//initialize all pointers to a null value
	m_OrthoMesh = nullptr;
	MCTerrain = nullptr;
	m_Texture = nullptr;
	m_DepthTexture = nullptr;
	m_PostProcessTexture = nullptr;
	m_CamDepthTexture = nullptr;
	m_TextureShader = nullptr;
	m_PostProcessShader = nullptr;
	m_Light = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	//store screen size in variables to hand in to post processing shader
	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;

	// Call inherited init function
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);


	//define a new terrain
	MCTerrain = new Terrain(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/rock.jpg", hwnd);
	MCTerrain->BuildTerrain(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/rock.jpg", ShapeNumber);

	//create new pointers to resources
	m_Texture = new Texture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/Grass.jpg");

	m_DepthTexture = new RenderTexture(m_Direct3D->GetDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);
	m_PostProcessTexture = new RenderTexture(m_Direct3D->GetDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);
	m_CamDepthTexture = new RenderTexture(m_Direct3D->GetDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);
	m_FirstBlurTexture = new RenderTexture(m_Direct3D->GetDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);
	m_SecondBlurTexture = new RenderTexture(m_Direct3D->GetDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);

	m_OrthoMesh = new OrthoMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 0, 0);

	m_TextureShader = new TextureShader(m_Direct3D->GetDevice(), hwnd);
	m_PostProcessShader = new PostProcessShader(m_Direct3D->GetDevice(), hwnd);
	m_HorzBlurShader = new ComputeShader(m_Direct3D->GetDevice(), hwnd, L"HorizontalBlur_cs.hlsl");
	m_VertBlurShader = new ComputeShader(m_Direct3D->GetDevice(), hwnd, L"VerticalBlur_cs.hlsl");

	m_Light = new Light;
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetAmbientColour(0.4f, 0.4f, 0.4f, 0.4f);
	m_Light->SetPosition(6.0f, 6.0f, 6.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);

	//generate light view and projection matrices for light calculations in shaders.
	m_Light->GenerateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
	m_Light->GenerateViewMatrix();
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_OrthoMesh)
	{
		delete m_OrthoMesh;
		m_OrthoMesh = 0;
	}

	// Release the Direct3D object.
	if (MCTerrain)
	{
		delete MCTerrain;
		MCTerrain = 0;
	}

	// Release the Direct3D object.
	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}

	// Release the Direct3D object.
	if (m_DepthTexture)
	{
		delete m_DepthTexture;
		m_DepthTexture = 0;
	}

	// Release the Direct3D object.
	if (m_PostProcessTexture)
	{
		delete m_PostProcessTexture;
		m_PostProcessTexture = 0;
	}

	// Release the Direct3D object.
	if (m_CamDepthTexture)
	{
		delete m_CamDepthTexture;
		m_CamDepthTexture = 0;
	}

	// Release the Direct3D object.
	if (m_TextureShader)
	{
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the Direct3D object.
	if (m_PostProcessShader)
	{
		delete m_PostProcessShader;
		m_PostProcessShader = 0;
	}

	// Release the Direct3D object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

}


bool App1::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

void App1::RenderTextures()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//start of rendertextures

	m_DepthTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	m_DepthTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	MCTerrain->DepthRender(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), worldMatrix, m_Light->GetViewMatrix(), m_Light->GetProjectionMatrix());

	m_Direct3D->ResetViewport();
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();



	m_CamDepthTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	m_CamDepthTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	MCTerrain->DepthRender(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);

	m_Direct3D->ResetViewport();
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	m_PostProcessTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	m_PostProcessTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	MCTerrain->Render(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Texture->GetTexture(), m_DepthTexture, m_Camera, m_Light);

	m_Direct3D->ResetViewport();
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();


	//end of rendertextures 
}

bool App1::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;

	//check if the shape number has changed since last frame (user has moved slider on GUI)
	if (PreviousShapeNumber != ShapeNumber)
	{
		MCTerrain->BuildTerrain(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/rock.jpg", ShapeNumber);
	}

	//m_Direct3D->TurnOnWireframe();

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//generate view frustum
	MCTerrain->Frame(20.0f, projectionMatrix, viewMatrix);

	RenderTextures();

	//m_Direct3D->GetDeviceContext()->CopyResource(m_FirstBlurTexture->m_renderTargetTexture, m_PostProcessTexture->m_renderTargetTexture);

	//for (int i = 0; i < 40; i++)
	//{
		//m_HorzBlurShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_PostProcessTexture->GetShaderResourceView(), m_FirstBlurTexture->GetUnorderedAccessView(), 4, 1);
	
		m_VertBlurShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_FirstBlurTexture->GetShaderResourceView(), m_PostProcessTexture->GetUnorderedAccessView(), 1024, 4);
	//}

	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_EVENT;
	desc.MiscFlags = 0;
	ID3D11Query* query = nullptr;
	if (SUCCEEDED(m_Direct3D->GetDevice()->CreateQuery(&desc, &query)))
	{
		m_Direct3D->GetDeviceContext()->End(query);
		BOOL data = 0;
		while (S_OK != m_Direct3D->GetDeviceContext()->GetData(query, &data, sizeof(data), 0))
		{
			// Wait f o r t h e GPU t o f i n i s h .
		}
		query->Release();
	}

	//m_Direct3D->ResetViewport();
	//// Reset the render target back to the original back buffer and not the render to texture anymore.
	//m_Direct3D->SetBackBufferRenderTarget();

	//render final image of scene to an orthographic mesh.
	m_Direct3D->GetOrthoMatrix(orthoMatrix);// ortho matrix for 2D rendering
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	m_Direct3D->TurnZBufferOff();
	
	m_OrthoMesh->SendData(m_Direct3D->GetDeviceContext());

	m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, m_PostProcessTexture->GetShaderResourceView());
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_OrthoMesh->GetIndexCount());

	m_Direct3D->TurnZBufferOn();

	PreviousShapeNumber = ShapeNumber;
	GUI();

	//// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void App1::GUI()
{
	// Force turn off on Geometry shader
	m_Direct3D->GetDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("Scene");
	ImGui::SliderInt("Shapes", &ShapeNumber, 0, 3);
	// Render UI
	ImGui::Render();
}


