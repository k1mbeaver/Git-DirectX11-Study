#pragma once
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// LINKING //
/////////////
// 가장 먼저 사용할 라이브러리를 지정해야한다.
// 첫번째 라이브러리는 DirectX 11에서 3D 그래픽을 설정하고 그리기 위한 모든 D3D 기능이 포함되어 있다.
// 두번째 라이브러리에는 모니터의 재생을, 세번째에는 다음 튜토리얼에서 다룰 셰이더 컴파일링 기능이 포함되어 있다.
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// 다음으로는 이 오브젝트 모듈에 연결된 라이브러리를 위한 헤더들과 DirectX에서 사용하는 타입 정의, 수학 기능을 인클루드합니다.
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

// 우선 가장 간단하게 만들어보자.
// 디폴트 생성자, 복사 생성자, 소멸자가 있다.
// 그 다음 초기화와 종료 기능이 있는데 이 튜토리얼에서 주로 다루게 될 중요한 기능이다.
// 그리고 그 외 중요하지 않은 일부 helper 함수와 D3DClass.cpp 파일을 검사할 때 필요한 private 멤버 변수가 있다.
// Initialize와 ShutDown에 초점을 맞춰서 공부하자.
////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif