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

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};