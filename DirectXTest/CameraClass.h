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
	// ����� ��� ��� ���� �ִ��� �˷��ִ� ī�޶� Ŭ����
	// ī�޶��� ��ġ�� ���� ȸ���� ����
	// ��ġ �� ȸ�� ������ ����Ͽ� �������� ���� HLSL ���̴��� ���޵� �� ��Ʈ������ �����Ѵ�.
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float); // ī�޶��� ��ġ ����
	void SetRotation(float, float, float); // ī�޶��� ȸ�� ����

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render(); // ī�޶��� ��ġ�� ȸ���� ������� �� ��Ʈ������ ����
	void GetViewMatrix(XMMATRIX&); // ���̴��� �������� ����� �� �ֵ��� ī�޶� ��ü���� �� ��Ʈ������ �˻��ϴµ� ���

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif