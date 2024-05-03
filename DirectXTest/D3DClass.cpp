#include "d3dclass.h"

// ���ó� ��� ��� �����͵��� null�� �ʱ�ȭ�ϴ� ������ ��������.
D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

// DirectX 11�� ��ü ������ �����Ѵ�.
// ���� Ʃ�丮���� ���� ��� �߰����� �׸�鵵 �����Ѵ�.
// screenWidth�� screenHeight�� SystemClass���� ������ â�� �ʺ�� �����̴�.
// Direct3D�� �̸� ����� â ũ�⸦ �����ϰ� �ʱ�ȭ�Ѵ�.
// hwnd ������ â�� ���� �ڵ��̴�. ������ ���� â�� �����ϱ� ���� �ʿ��ϴ�.
// fullScreen ������ â������� ��üȭ�� ��������� �ǹ��Ѵ�.
// screenDepth�� screenNear�� �������� 3D ȯ�濡 ���� ���� �����̴�.
// vsync�� ����� ������� ���ΰ�ħ �󵵿� ���� ���������� Ȥ�� ������ �� ������ ������������ �ǹ��Ѵ�.
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// �ʱ�ȭ�� �ϱ� ���� ���� ī��� ����Ϳ��� ��� �󵵸� �����´�.
	// ��ǻ�͸��� ���ݾ� �ٸ��Ƿ� �ش� ������ �����Ѵ�.
	// Direct3d�� ���� �������ָ� ������ ��� �󵵰� ���ȴ�.
	// �� �۾��� �ʼ����̸� �⺻������ �����ϸ� ���� �ø� ��� blit�� ����Ǿ� ������ ���ϵǰ� ������ �߻��Ѵ�.

	// DirectX �׷��Ƚ� �������̽� ���丮 ����
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// factory�� ����� �⺻ �׷��� �������̽�(���� ī��)�� ���� ����͸� �����.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// �⺻ ����� ���(�����)�� �����Ѵ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// ����� ���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ���÷��� ���Ŀ� �´� ��� ���� �����´�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// �� �����/���� ī�� ���տ� ���� ������ ��� ���÷��� ��尡 ���� ����Ʈ�� ����ϴ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ���� DisplayModeList�� ä���.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ��� ���÷��� ��带 ���캸�� ȭ�� �ʺ�� ���̿� ��ġ�ϴ� ��带 ã���ϴ�.
	// ��ġ�ϴ� �׸��� �߰ߵǸ� �ش� ����Ϳ� ���� ��� ���� numerator�� denominator�� �����մϴ�.
	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// ���� ��� �󵵸� ���� numerator�� denomiator�� �غ�Ǿ���.
	// ���������� ����͸� ����� ���� ī���� �̸��� ���� �޸��� ���� �����´�.

	// �����(���� ī��) description�� �����´�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ���� ī�� �޸𸮸� mg ������ �����Ѵ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ���� ī�� �̸��� ĳ���� �迭�� ��ȯ�Ͽ� �����Ѵ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// ��� �󵵿� ���� ī�� ������ ���������Ƿ� ���� �׿� ���� �ڿ��� �����Ѵ�.

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// ���� DirectX �ʱ�ȭ�� �����Ѵ�.
	// ���� ���� swap chain descriptiton�� �ۼ��Ѵ�.
	// ���� ü���̶� �׷����� �׷��� ���� �� �ĸ� �����̴�.
	// �Ϲ������� �ϳ��� �� ���۸� ����� ��� ������� �����ϰ�, �� ���� ���� ���۷� ��ü�Ǵ� ���̴�.
	// ��� ��ü�Ǳ� ������ ���� ü���̶�� �Ѵ�.

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �������� refresh rate description�� �ۼ��Ѵ�.
	// refresh rate(��� ��)�� 1�ʿ� ����۰� ����Ʈ ���۷� ����̳� �׷��� �������� �ǹ��Ѵ�.
	// vsync�� true�̸� ��� �󵵰� �ý��� ����(ex:60������)���� ��������.
	// false�� �����ϸ� �ʴ� ������ ������ �׷��� �Ϻ� �ð��� ȿ���� �߻��ϱ⵵ �Ѵ�.

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// swap chain description�� �ۼ��� �� feature level�� ���õǾ�� �Ѵ�.
	// �� ������ �츮�� ����� DirectX ������ �ǹ��Ѵ�.
	// ���⼭�� 11.0 �� DirectX 11�� �����Ѵ�
	// ���� ������ �����ϰų� ����� �ϵ����� �����ϰ� ���� ��� 10 �Ǵ� 9�� ������ �� ���� ������ ����� �� �ִ�.

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// �� ���� D3D ��ġ�� ��ġ ���ý�Ʈ�� �����.
	// �̴� D3D ����� �������̽��� �ǹ��Ѵ�. �������� ���� ��� ��ɿ� �� �ΰ����� �ʿ��ϴ�.
	// ���� ������ D3D�� �ͼ��� ����� ��ġ ���ý�Ʈ�� �ͼ����� �����ٵ� ������ ����� �� �ΰ����� �ɰ��� ���̹Ƿ� �Ѵ� �˵��� ����.
	// ���� D3D 11�� �����ϴ� ���� ī�尡 ���� ��� ���⼭ ��ġ�� ��ġ ���ý�Ʈ�� �����ϴµ� �����Ѵ�.
	// ���� D3D 11�� �׽�Ʈ�ϰų� �����ϴ� ����ī�尡 ���� ��쿣  D3D_DRIVER_TYPE_HARDWARE�� D3D_DRIVER_TYPE_REFERENCE�� �����Ͽ� CPU�� ��� �������ϵ��� ���� �� �ִ�.
	// ��� �ӵ��� 1/1000 ������ ���� ���� ī�尡 ���ٸ� ����.

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// �⺻ ���� ī�尡 DX11�� ȣȯ���� �ʴ� ��� ��ġ�� ������ ���ϰ� �����ϴ� ��찡 �ִ�.
	// �Ϻ� �ý��ۿ����� �⺻ ī�尡 DX10���̰� ���� ī�尡 DX11���̰ų�,
	// �� � �ý��ۿ����� �⺻�� Intel ������ �׷��� ī��� ������ ���� NVidia ī��� �Ǿ��ִ�.
	// �� ������ �ذ��ϱ� ���� �⺻ ��ġ�� ������� �ʰ� ��ǻ���� ��� ���� ī�� �� ����ڰ� ����� ī�带 ������ �����ϵ��� �ؾ��Ѵ�.

	// ���� ����ü���� �����Ƿ� �� ���� �����͸� ���⿡ ��������.
	// ���⼭ CreateRenderTargetView �Լ��� ����Ѵ�.

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// ���� Depth buffer description�� �ʱ�ȭ�Ѵ�.
	// �������� 3D �������� �����ϰ� �������� �� �ֵ��� �ϱ� ���� ����ϴ� �����̴�.
	// ���ÿ� ���ٽ� ���� ���� ���� ���ۿ� �����Ѵ�.
	// ���ٽ� ���۴� ��� ��, ���� �׸��� ����� ȿ���� ��µ� ����� �� �ִ�.

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ���� �ش� description�� ���� ����/���ٽ� ���۸� �����Ѵ�.
	// CreateTexture2D�� Ȱ���Ѵ�. �̸� ���� ���۴� 2D �ؽ�ó�� ���̶�� ���� �� �� �ִ�.

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� depth stencil description�� �����Ѵ�.
	// �̸� ���� D3D�� �� �ȼ��� ���� ������ ���� �׽�Ʈ ������ ������ �� �ִ�.

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� depth sctencil state�� �����Ѵ�.

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// ��ġ ���ý�Ʈ�� �̿��� ������ depth stencil state�� �����Ų��.

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// D3D�� depth buffer�� depth stencil texture�� ����Ѵٴ� ����� �˵��� ���ٽ� description�� �ۼ��Ѵ�.

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// ���� OMSetRenderTargets�� ȣ���� ���� Ÿ�� ��� ���� ���ٽ� ���۸� ������ ���������ο� ���ε��Ѵ�.
	// �̷��� �ϸ� ���������ο��� �������Ǵ� �׷����� �� ���ۿ� ��ϵȴ�.
	// �� �� ���� ���ۿ� ��ü�Ǿ� ����� ȭ�鿡 �׷����� ǥ�õȴ�.

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	// Direct3D�� Ŭ�� ���� ��ǥ�� ������ ��� ������ ������ �� �ֵ��� ����Ʈ�� �����ؾ� �Ѵ�. 
	// â�� ��ü ũ��� �����Ѵ�.

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth / 2;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(2, &viewport);

	// ���� ���� ����� �����. ���� ����� 3D ����� ������ ���� 2D ����Ʈ �������� ��ȯ�ϴµ� ���ȴ�.
	// ����� �������ϴµ� ����� ���̴��� �����ؾ� �ϹǷ� ���纻�� �����Ѵ�.

	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// ���� ���� ����� �����. �� ����� ��ü�� ������ 3D �� �������� ��ȯ�ϴ� �� ���ȴ�
	// �켱 ������ķ� �ʱ�ȭ�� �����Ѵ�. ���̴����� ������ ���̴�.

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// ���� ���⼭ �� ����� �����. �� ����� ���� �����ִ� ī�޶� ��ġ�� ����ϴµ� ���ȴ�
	// ���� Ʃ�丮���� ī�޶� Ŭ�������� ���鵵�� ����.

	// ���������� ���� ���� ����� �����.

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

// ShutDown �Լ��� ��� �����͸� �����ϰ� �����Ѵ�.
// ������ ���� ü���� â ���� ��ȯ�Ѵ�.
// ���� �� ���� ���� ü���� �����Ǹ� ��� ���ܰ� �߻��ϱ� �����̴�.
// ���� D3D�� �����ϱ� ���� ������ â ��带 �����ϵ��� ����.
void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

// D3DClass���� ��� helper �Լ��� �ִ�.
// ���� BeginScene�� EndScene��� �ΰ��� �Լ��� �ִ�.
// BeginScene�� �� ������ ���ۿ� ���ο� ���� �׸������� ȣ��ȴ�.
// ���۸� �ʱ�ȭ�Ͽ� �׸� �غ� �ϴ� ���̴�.
// �� ���� ������ ����ü�ο� ���� ǥ���ϵ��� �����Ѵ�.
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

// ������ D3D ��ġ�� ��ġ ���ý�Ʈ�� �����͸� �������� ������ �Լ��̴�.
ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

// ���� �� �Լ��� ����, ����, ���� ����� ���纻�� ��� �Լ��̴�.
// ��κ��� ���̴��� �������� ���� ���� �� ����� �ʿ��ϴ�.
void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

// �������� ���� ī���� �̸��� ���� �޸� ���� ������ ��ȯ�Ѵ�.
// ���� ī���� �̸��� ����뿡 ������ �� �� �ִ�.
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}