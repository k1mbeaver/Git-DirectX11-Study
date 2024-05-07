#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
	// 장면을 어디서 어떻게 보고 있는지 알려주는 카메라 클래스
	// 카메라의 위치와 현재 회전을 추적
	// 위치 및 회전 정보를 사용하여 렌더링을 위해 HLSL 셰이더에 전달될 뷰 매트릭스를 생성한다.
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float); // 카메라의 위치 설정
	void SetRotation(float, float, float); // 카메라의 회전 설정

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render(); // 카메라의 위치와 회전을 기반으로 뷰 매트릭스를 생성
	void GetViewMatrix(XMMATRIX&); // 셰이더가 렌더링에 사용할 수 있도록 카메라 객체에서 뷰 매트릭스를 검색하는데 사용

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif