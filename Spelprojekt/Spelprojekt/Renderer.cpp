#include "Renderer.h"


Renderer::Renderer(size_t width, size_t height, Timer& timer, DX11Handler& dx11) : dx11(dx11), timer(timer), lights(width, height, 64, 64), ssao(width, height), gbuffersampler(nullptr)
{
	this->currentRenderTarget = nullptr;
	this->meshMat = nullptr;

	this->gbufferRenderTarget = new RenderTarget(4, width, height, true);
	this->gbufferRenderTarget->Initalize(dx11.GetDevice());

	this->backbufferRenderTarget = dx11.GetBackbufferRenderTarget();
	this->gui = nullptr;
	this->deferredLightShader = new Shader();
	this->deferredLightShader->LoadVertexShader(L"Shaders/ScreenQuad_vs.hlsl", "main", dx11.GetDevice());
	this->deferredLightShader->LoadPixelShader(L"Shaders/DeferredLightPass_ps.hlsl", "main", dx11.GetDevice());

	screenQuad = MeshCreator::CreateScreenQuad(dx11.GetDevice());
	worldBuffer_ptr = dx11.CreateBuffer<WorldData>(cb_world);
	gbuffersampler = dx11.CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	this->shadowRenderTarget = new RenderTarget(0, (size_t)SHADOW_MAP_SIZE, (size_t)SHADOW_MAP_SIZE, true);
	this->shadowRenderTarget->Initalize(dx11.GetDevice());

	
	this->shadowShader = new Shader();
	this->shadowShader->LoadPixelShader(L"Shaders/Shadow_ps.hlsl", "main", dx11.GetDevice());
	this->shadowShader->LoadVertexShader(L"Shaders/Shadow_vs.hlsl", "main", dx11.GetDevice());

	this->ssao.Initialize(&dx11);
	this->lights.Initialize(&dx11);
}

Renderer::~Renderer() {}

void Renderer::SetDeferredRenderTarget()
{
	this->gbufferRenderTarget->Unbind(dx11.GetContext());
	SetRenderTarget(gbufferRenderTarget);
}

void Renderer::SetRenderTarget(RenderTarget* renderTarget)
{
	this->currentRenderTarget = renderTarget;

	dx11.GetContext()->RSSetViewports(1, &currentRenderTarget->GetViewport());
	dx11.GetContext()->OMSetRenderTargets((UINT)currentRenderTarget->BufferCount(), currentRenderTarget->GetRenderTargetViews(), currentRenderTarget->GetDepthStencil());

	if (currentRenderTarget->GetDepthStencilState() != nullptr)
		dx11.GetContext()->OMSetDepthStencilState(currentRenderTarget->GetDepthStencilState(), 0);
}

void Renderer::ClearRenderTarget()
{
	for (size_t i = 0; i < currentRenderTarget->BufferCount(); i++)
		dx11.GetContext()->ClearRenderTargetView(currentRenderTarget->GetRenderTargetViews()[i], CLEAR_COLOR);

	if (currentRenderTarget->GetDepthStencil() != nullptr)
		dx11.GetContext()->ClearDepthStencilView(currentRenderTarget->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::DrawMesh(Mesh* mesh, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 centre)
{
	// update the world buffer content
	cb_world.mvp = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, view), projection));
	cb_world.world = DirectX::XMMatrixTranspose(world);
	cb_world.time = static_cast<float>(timer.GetMilisecondsElapsed()) / 1000.0f;
	cb_world.cameraRight = right;
	cb_world.cameraUp = up;
	cb_world.centre = centre;
	cb_world.vp = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(view, projection));
	cb_world.invView = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, view));
	cb_world.invWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, world));

	// Check if the mesh has a valid skeleton and if the skeleton has an animation
	if (mesh->skeleton && mesh->skeleton->animations.size() > 0)
	{
		// Update the constant buffer with the final transformation of every bone in the skeleton, at the current keyframe
		for (unsigned int i = 0; i < mesh->skeleton->GetNumberOfBones(); i++)
		{
			cb_world.boneTransforms[i] = mesh->skeleton->GetCurrentAnimation()->GetBone(i).GetFinalTransformation((unsigned int)mesh->skeleton->GetKeyframe());
		}
	}


	dx11.GetContext()->UpdateSubresource(worldBuffer_ptr, 0, 0, &cb_world, 0, 0);
	dx11.GetContext()->VSSetConstantBuffers(WORLD_CONSTANT_BUFFER_SLOT, 1, &worldBuffer_ptr);

	DrawMesh(mesh);
}


void Renderer::ShadowPass(DirectX::XMVECTOR focus, const AABB& bounds, Camera* camera, Entities* entities)
{
	dx11.GetContext()->RSSetState(dx11.GetShadowRasterizerState());
	SetRenderTarget(shadowRenderTarget);
	ClearRenderTarget();

	lights.UpdateCameras(focus, bounds);
	shadowShader->Bind(dx11.GetContext());
	Camera* sunCamera = lights.GetSunCamera();

	std::vector<Object*> objects = entities->GetObjectsInView(sunCamera);

	// check if insight
	for (auto object : objects)
	{
		if (object->IsEnabled())
		{
			DirectX::XMFLOAT3 right;
			DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(camera->GetTransform().Right()));
			DirectX::XMFLOAT3 up;
			DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(camera->GetTransform().Up()));
			object->Render(this, sunCamera->GetView(), sunCamera->GetOrthographic(), right, up);
		}
	}

	// BIND AND STUFF

	// revert back to main rasterizer
	dx11.GetContext()->RSSetState(dx11.GetMainRasterizerState());
}


void Renderer::DisplayFrame(Camera* camera)
{
	//Uppdate light constant buffer 
	ssao.Pass(this, gbufferRenderTarget);
	lights.UpdateConstantBuffer(camera, dx11.GetContext());

	SetRenderTarget(backbufferRenderTarget);
	ClearRenderTarget();

	gbufferRenderTarget->Bind(dx11.GetContext());
	deferredLightShader->Bind(dx11.GetContext());

	//// bind ssao output texture
	ID3D11ShaderResourceView* ssaoSRV = ssao.GetOutputSRV();
	dx11.GetContext()->PSSetShaderResources((UINT)gbufferRenderTarget->BufferCount(), 1, &ssaoSRV);
	dx11.GetContext()->PSSetSamplers(0, 1, &gbuffersampler);
	 
	ID3D11ShaderResourceView* shadowSRV = shadowRenderTarget->GetDepthSRV();
	dx11.GetContext()->PSSetShaderResources((UINT)gbufferRenderTarget->BufferCount() + 1, 1, &shadowSRV);

	DrawScreenQuad();

	//// unbinds ssao_random texture
	ID3D11ShaderResourceView* pSRV[1] = { NULL };
	ID3D11SamplerState* ssrf[1] = { NULL };
	dx11.GetContext()->PSSetShaderResources((UINT)gbufferRenderTarget->BufferCount(), 1, pSRV);
	dx11.GetContext()->PSSetShaderResources((UINT)gbufferRenderTarget->BufferCount() + 1, 1, pSRV);

	// GUI PASS
	if (gui != nullptr)
		gui->DrawAll();

	dx11.GetSwapChain()->Present(vSync, 0);
}


void Renderer::DrawScreenQuad()
{
	DrawMesh(screenQuad);
}

void Renderer::DrawMesh(Mesh* mesh)
{
	UINT stride = sizeof(MeshVertex);
	UINT offset = 0;

	dx11.GetContext()->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
	dx11.GetContext()->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dx11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dx11.GetContext()->DrawIndexed((UINT)mesh->indices.size(), 0, 0);
}