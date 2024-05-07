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

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};