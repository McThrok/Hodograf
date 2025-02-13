#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	InitGui(hwnd);

	return true;
}
void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.05f, 0.05f, 0.1f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	RenderVisualisation();
	RendeGui();

	this->swapchain->Present(0, NULL);
}
void Graphics::InitGui(HWND hwnd) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();
}

void Graphics::RendeGui() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderMainPanel();
	RenderPositionChart();
	RenderVelocityChart();
	RenderAccelerationChart();
	RenderStateChart();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void Graphics::RenderMainPanel() {
	ImGui::SetNextWindowSize(ImVec2(400, 410), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	if (!ImGui::Begin("Main Panel"))
	{
		ImGui::End();
		return;
	}

	if (simulation->paused) {
		if (ImGui::Button("Start"))
			simulation->paused = false;
	}
	else {
		if (ImGui::Button("Pause"))
			simulation->paused = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		simulation->Reset();
	}

	ImGui::SliderFloat("L", &simulation->L, 1, 20, "%.4f");
	ImGui::SliderFloat("R", &simulation->R, 1, 20, "%.4f");
	ImGui::SliderFloat("w", &simulation->omega, 0, 3.14, "%.4f");
	ImGui::SliderFloat("e0", &simulation->e0, 0, 0.1, "%.4f");
	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.01f, 0.05f, "%.4f");
	ImGui::SliderFloat("simulation speed", &simulation->simulationSpeed, 0, 10);
	ImGui::SliderInt("diff offset", &simulation->diffOffset, 1,10);

	ImGui::Separator();
	ImGui::DragFloat2("min position", &simulation->minX.x,0.1f);
	ImGui::DragFloat2("max position", &simulation->maxX.x, 0.1f);
	ImGui::DragFloat2("min velocity", &simulation->minXt.x, 0.1f);
	ImGui::DragFloat2("max velocity", &simulation->maxXt.x, 0.1f);
	ImGui::DragFloat2("min acceleration", &simulation->minXtt.x, 0.1f);
	ImGui::DragFloat2("max acceleration", &simulation->maxXtt.x, 0.1f);
	ImGui::DragFloat2("min state", &simulation->minState.x, 0.1f);
	ImGui::DragFloat2("max state", &simulation->maxState.x, 0.1f);

	ImGui::End();
}
void Graphics::RenderPositionChart()
{
	ImGui::SetNextWindowSize(ImVec2(1470, 220), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(420, 310), ImGuiCond_Once);
	if (!ImGui::Begin("position"))
	{
		ImGui::End();
		return;
	}

	auto data = ChartData(&(simulation->x), IM_COL32(200, 0, 0, 255));
	MyImGui::DrawChart({ data }, simulation->minX, simulation->maxX);

	ImGui::End();

}
void Graphics::RenderVelocityChart()
{
	ImGui::SetNextWindowSize(ImVec2(1470, 220), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(420, 540), ImGuiCond_Once);
	if (!ImGui::Begin("velocity"))
	{
		ImGui::End();
		return;
	}

	auto data = ChartData(&(simulation->xt), IM_COL32(200, 0, 0, 255));
	MyImGui::DrawChart({ data }, simulation->minXt, simulation->maxXt);

	ImGui::End();

}
void Graphics::RenderAccelerationChart()
{
	ImGui::SetNextWindowSize(ImVec2(1470, 220), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(420, 770), ImGuiCond_Once);
	if (!ImGui::Begin("acceleration"))
	{
		ImGui::End();
		return;
	}

	auto data = ChartData(&(simulation->xtt), IM_COL32(200, 0, 0, 255));
	MyImGui::DrawChart({ data }, simulation->minXtt, simulation->maxXtt);

	ImGui::End();

}
void Graphics::RenderStateChart()
{
	ImGui::SetNextWindowSize(ImVec2(400, 560), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 430), ImGuiCond_Once);
	if (!ImGui::Begin("state"))
	{
		ImGui::End();
		return;
	}

	auto data = ChartData(&(simulation->state), IM_COL32(200, 0, 0, 255));
	MyImGui::DrawChart({ data }, simulation->minState, simulation->maxState);

	ImGui::End();

}
void Graphics::RenderVisualisation()
{
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pureColorPixelshader.GetShader(), NULL, 0);

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());

	vector<Matrix> matrices = GetMatrices();
	RenderSquare({ 0.8f ,0.8f ,0.8f ,1 }, matrices[0]);
	RenderSquare({ 0.8f ,0.8f ,0.8f ,1 }, matrices[1]);
	RenderSquare({ 0.8f ,0.8f ,0.8f ,1 }, matrices[2]);
}
void Graphics::RenderSquare(Vector4 color, Matrix matrix)
{
	UINT offset = 0;

	cbColoredObject.data.worldMatrix = matrix;
	cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbColoredObject.data.color = color;

	if (!cbColoredObject.ApplyChanges()) return;
	deviceContext->IASetVertexBuffers(0, 1, vbSquare.GetAddressOf(), vbSquare.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(ibSquare.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(ibSquare.BufferSize(), 0, 0);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXGI Adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = this->windowWidth;
	scd.BufferDesc.Height = this->windowHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, //FOR SOFTWARE DRIVER TYPE
		NULL, //FLAGS FOR RUNTIME LAYERS
		NULL, //FEATURE LEVELS ARRAY
		0, //# OF FEATURE LEVELS IN ARRAY
		D3D11_SDK_VERSION,
		&scd, //Swapchain description
		this->swapchain.GetAddressOf(), //Swapchain Address
		this->device.GetAddressOf(), //Device Address
		NULL, //Supported feature level
		this->deviceContext.GetAddressOf()); //Device Context Address

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = this->windowWidth;
	depthStencilDesc.Height = this->windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = this->windowWidth;
	viewport.Height = this->windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	return true;
}
bool Graphics::InitializeShaders()
{
	wstring root = L"";

	if (!vertexshader.Initialize(this->device, root + L"my_vs.cso", VertexP::layout, ARRAYSIZE(VertexP::layout))) return false;
	if (!pureColorPixelshader.Initialize(this->device, root + L"pureColor_ps.cso")) return false;

	return true;
}

void Graphics::InitSquare()
{
	VertexP v[] =
	{
		{-0.5f,  -0.5f, 0.0f},
		{-0.5f,   0.5f, 0.0f},
		{0.5f ,   0.5f,  0.0f},
		{0.5f ,  -0.5f,  0.0f},
	};

	this->vbSquare.Initialize(this->device.Get(), v, ARRAYSIZE(v));

	int indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	this->ibSquare.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));

}
bool Graphics::InitializeScene()
{
	InitSquare();

	cbColoredObject.Initialize(device.Get(), deviceContext.Get());

	camera.SetPosition(5, -10, -5);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}

vector<Matrix> Graphics::GetMatrices()
{
	vector<Matrix> result(3);
	float armWidth = 0.3;
	float a = simulation->alpha;
	float r = simulation->R;
	float l = simulation->disturbed_L;
	float x = simulation->x[simulation->x.size() - 1].y;

	Matrix mBase = Matrix::CreateRotationX(XM_PIDIV2) * Matrix::CreateTranslation(0.5, 0, 0);
	result[0] = mBase * Matrix::CreateScale(r, 1, armWidth) * Matrix::CreateRotationY(a);
	float b = XM_PIDIV2 - atan2f(x - cosf(a) * r, sinf(a) * r);
	result[1] = mBase * Matrix::CreateScale(l, 1, armWidth) * Matrix::CreateRotationY(-b) * Matrix::CreateTranslation(cosf(a) * r, 0, sinf(-a) * r);
	result[2] = mBase * Matrix::CreateTranslation(x, 0, 0);

	return result;
}
