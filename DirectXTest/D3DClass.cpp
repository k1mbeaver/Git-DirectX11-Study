#include "d3dclass.h"

// 역시나 모든 멤버 포인터들을 null로 초기화하는 것으로 시작하자.
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

// DirectX 11을 전체 설정을 수행한다.
// 추후 튜토리얼을 위한 몇가지 추가적인 항목들도 존재한다.
// screenWidth와 screenHeight는 SystemClass에서 생성한 창의 너비와 높이이다.
// Direct3D도 이를 사용해 창 크기를 동일하게 초기화한다.
// hwnd 변수는 창에 대한 핸들이다. 이전에 만든 창에 접근하기 위해 필요하다.
// fullScreen 변수는 창모드인지 전체화면 모드인지를 의미한다.
// screenDepth와 screenNear은 렌더링될 3D 환경에 대한 깊이 설정이다.
// vsync는 사용자 모니터의 새로고침 빈도에 따라 렌더링할지 혹은 가능한 한 빠르게 렌더링할지를 의미한다.
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

	// 초기화를 하기 전에 비디오 카드와 모니터에서 재생 빈도를 가져온다.
	// 컴퓨터마다 조금씩 다르므로 해당 정보를 쿼리한다.
	// Direct3d에 값을 전달해주면 적절한 재생 빈도가 계산된다.
	// 이 작업은 필수적이며 기본값으로 설정하면 버퍼 플립 대신 blit이 수행되어 성능이 저하되고 오류가 발생한다.

	// DirectX 그래픽스 인터페이스 팩토리 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// factory를 사용해 기본 그래픽 인터페이스(비디오 카드)를 위한 어댑터를 만든다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 기본 어댑터 출력(모니터)을 열거한다.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 어댑터 출력(모니터)을 위해 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 형식에 맞는 모드 수를 가져온다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 이 모니터/비디오 카드 조합에 대해 가능한 모든 디스플레이 모드가 적힌 리스트를 만듭니다.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 이제 DisplayModeList를 채운다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 모든 디스플레이 모드를 살펴보고 화면 너비와 높이와 일치하는 모드를 찾습니다.
	// 일치하는 항목이 발견되면 해당 모니터에 대한 재생 빈도의 numerator와 denominator을 저장합니다.
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

	// 이제 재생 빈도를 위한 numerator와 denomiator가 준비되었다.
	// 마지막으로 어댑터를 사용해 비디오 카드의 이름과 비디오 메모리의 양을 가져온다.

	// 어댑터(비디오 카드) description을 가져온다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 전용 비디오 카드 메모리를 mg 단위로 저장한다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오 카드 이름을 캐릭터 배열로 변환하여 저장한다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 재생 빈도와 비디오 카드 정보를 저장했으므로 이제 그에 사용된 자원을 해제한다.

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

	// 이제 DirectX 초기화를 시작한다.
	// 가장 먼저 swap chain descriptiton을 작성한다.
	// 스왑 체인이란 그래픽이 그려질 전면 및 후면 버퍼이다.
	// 일반적으로 하나의 백 버퍼를 사용해 모든 드로잉을 수행하고, 그 다음 전면 버퍼로 교체되는 식이다.
	// 계속 교체되기 때문에 스왑 체인이라고 한다.

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 다음으로 refresh rate description을 작성한다.
	// refresh rate(재생 빈도)란 1초에 백버퍼가 프론트 버퍼로 몇번이나 그려낼 것인지를 의미한다.
	// vsync가 true이면 재생 빈도가 시스템 설정(ex:60프레임)으로 맞춰진다.
	// false로 설정하면 초당 가능한 빠르게 그려서 일부 시각적 효과가 발생하기도 한다.

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

	// swap chain description을 작성한 후 feature level이 세팅되어야 한다.
	// 이 변수는 우리가 사용할 DirectX 버전을 의미한다.
	// 여기서는 11.0 즉 DirectX 11로 설정한다
	// 여러 버전을 지원하거나 저사양 하드웨어에서 실행하고 싶은 경우 10 또는 9로 설정해 더 낮은 버전을 사용할 수 있다.

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 그 다음 D3D 장치와 장치 컨택스트를 만든다.
	// 이는 D3D 기능의 인터페이스를 의미한다. 이제부터 거의 모든 기능에 이 두가지가 필요하다.
	// 이전 버전의 D3D에 익숙한 사람은 장치 컨택스트에 익숙하지 않을텐데 기존의 기능이 이 두가지로 쪼개진 것이므로 둘다 알도록 하자.
	// 만약 D3D 11을 지원하는 비디오 카드가 없는 경우 여기서 장치와 장치 컨택스트를 생성하는데 실패한다.
	// 직접 D3D 11을 테스트하거나 지원하는 비디오카드가 없는 경우엔  D3D_DRIVER_TYPE_HARDWARE를 D3D_DRIVER_TYPE_REFERENCE로 변경하여 CPU가 대신 렌더링하도록 만들 수 있다.
	// 비록 속도는 1/1000 이지만 아직 비디오 카드가 없다면 참고.

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 기본 비디오 카드가 DX11과 호환되지 않는 경우 장치를 만들지 못하고 실패하는 경우가 있다.
	// 일부 시스템에서는 기본 카드가 DX10용이고 보조 카드가 DX11용이거나,
	// 또 어떤 시스템에서는 기본이 Intel 저전력 그래픽 카드고 보조가 고성능 NVidia 카드로 되어있다.
	// 이 문제를 해결하기 위해 기본 장치를 사용하지 않고 컴퓨터의 모든 비디오 카드 중 사용자가 사용할 카드를 선택해 지정하도록 해야한다.

	// 이제 스왑체인이 있으므로 백 버퍼 포인터를 여기에 연결하자.
	// 여기서 CreateRenderTargetView 함수를 사용한다.

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

	// 이제 Depth buffer description을 초기화한다.
	// 폴리곤이 3D 공간에서 적절하게 렌더링될 수 있도록 하기 위해 사용하는 버퍼이다.
	// 동시에 스텐실 버퍼 또한 깊이 버퍼에 연결한다.
	// 스텐실 버퍼는 모션 블러, 볼륨 그림자 등등의 효과를 얻는데 사용할 수 있다.

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

	// 이제 해당 description을 통해 깊이/스텐실 버퍼를 생성한다.
	// CreateTexture2D를 활용한다. 이를 통해 버퍼는 2D 텍스처일 뿐이라는 것을 알 수 있다.

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 depth stencil description을 설정한다.
	// 이를 통해 D3D가 각 픽셀에 대해 수행할 깊이 테스트 유형을 설정할 수 있다.

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

	// 이제 depth sctencil state를 생성한다.

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 장치 컨택스트를 이용해 생성된 depth stencil state로 적용시킨다.

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// D3D가 depth buffer을 depth stencil texture로 사용한다는 사실을 알도록 스텐실 description을 작성한다.

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

	// 이제 OMSetRenderTargets를 호출해 렌더 타겟 뷰와 깊이 스텐실 버퍼를 렌더링 파이프라인에 바인딩한다.
	// 이렇게 하면 파이프라인에서 렌더링되는 그래픽이 백 버퍼에 기록된다.
	// 그 후 전면 버퍼와 교체되어 사용자 화면에 그래픽이 표시된다.

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

	// Direct3D가 클립 공간 좌표를 렌더링 대상 공간에 매핑할 수 있도록 뷰포트도 설정해야 한다. 
	// 창의 전체 크기로 설정한다.

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth / 2;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(2, &viewport);

	// 이제 투영 행렬을 만든다. 투영 행렬은 3D 장면을 이전에 만든 2D 뷰포트 공간으로 변환하는데 사용된다.
	// 장면을 렌더링하는데 사용할 셰이더에 전달해야 하므로 복사본을 보관한다.

	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 이제 월드 행렬을 만든다. 이 행렬은 객체의 정점을 3D 씬 기준으로 변환하는 데 사용된다
	// 우선 단위행렬로 초기화해 보관한다. 셰이더에도 전달할 것이다.

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// 원랜 여기서 뷰 행렬을 만든다. 이 행렬은 씬을 보고있는 카메라 위치를 계산하는데 사용된다
	// 추후 튜토리얼의 카메라 클래스에서 만들도록 하자.

	// 마지막으로 직교 투영 행렬을 만든다.

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

// ShutDown 함수는 모든 포인터를 해제하고 정리한다.
// 그전에 스왑 체인을 창 모드로 전환한다.
// 만약 그 전에 스왑 체인이 해제되면 몇가지 예외가 발생하기 때문이다.
// 따라서 D3D를 종료하기 전에 무조건 창 모드를 실행하도록 하자.
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

// D3DClass에는 몇가지 helper 함수가 있다.
// 먼저 BeginScene과 EndScene라는 두가지 함수가 있다.
// BeginScene은 각 프레임 시작에 새로운 씬을 그릴때마다 호출된다.
// 버퍼를 초기화하여 그릴 준비를 하는 것이다.
// 다 끝난 다음엔 스왑체인에 씬을 표시하도록 지시한다.
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

// 다음은 D3D 장치와 장치 컨택스트의 포인터를 가져오는 간단한 함수이다.
ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

// 다음 세 함수는 투영, 월드, 직교 행렬의 복사본을 얻는 함수이다.
// 대부분의 셰이더는 렌더링을 위해 다음 세 행렬이 필요하다.
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

// 마지막은 비디오 카드의 이름과 비디오 메모리 양을 참조로 반환한다.
// 비디오 카드의 이름이 디버깅에 도움이 될 수 있다.
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}