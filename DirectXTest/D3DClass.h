#pragma once
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// LINKING //
/////////////
// ���� ���� ����� ���̺귯���� �����ؾ��Ѵ�.
// ù��° ���̺귯���� DirectX 11���� 3D �׷����� �����ϰ� �׸��� ���� ��� D3D ����� ���ԵǾ� �ִ�.
// �ι�° ���̺귯������ ������� �����, ����°���� ���� Ʃ�丮�󿡼� �ٷ� ���̴� �����ϸ� ����� ���ԵǾ� �ִ�.
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// �������δ� �� ������Ʈ ��⿡ ����� ���̺귯���� ���� ������ DirectX���� ����ϴ� Ÿ�� ����, ���� ����� ��Ŭ����մϴ�.
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

// �켱 ���� �����ϰ� ������.
// ����Ʈ ������, ���� ������, �Ҹ��ڰ� �ִ�.
// �� ���� �ʱ�ȭ�� ���� ����� �ִµ� �� Ʃ�丮�󿡼� �ַ� �ٷ�� �� �߿��� ����̴�.
// �׸��� �� �� �߿����� ���� �Ϻ� helper �Լ��� D3DClass.cpp ������ �˻��� �� �ʿ��� private ��� ������ �ִ�.
// Initialize�� ShutDown�� ������ ���缭 ��������.
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