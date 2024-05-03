#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// ���⼭ WIN32_LEAN_AND_MEAN�� �����Ѵ�.
// �̴� ���� �ӵ��� ���� �� �ִµ�, �� ���̴� API���� ���������ν� win32 ��� ������ ũ�⸦ ���ҽ�Ų��.
///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

// SystemClass�� ����� �� �ֵ��� ���⼭ �����ӿ�ũ�� �ٸ� �� Ŭ������ ��Ŭ����Ѵ�.
//////////////
// INCLUDES //
//////////////
#include <windows.h>
//Windows.h�� ��Ŭ�������ν� �츮�� window�� ����/�ı��� �� �ְ� �ٸ� ������ win32 �Լ����� ����� �� �ִ�.

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "InputClass.h"
#include "GraphicsClass.h"
// class�� ���ǰ� ������ �ܼ��ϴ�.
// WinMain���� ȣ��Ǵ� Initialize, ShutDown, Run �Լ��� Ȯ���� �� �ִ�.
// �׸��� �ռ� �Ұ��ߴ� m_Input�� m_Graphics ��ü�� ����Ű�� �����Ͱ� private ����� ����� ���� Ȯ���� �� �ִ�.

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
};


/////////////////////////
// �Լ� PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// �������� //
/////////////
static SystemClass* ApplicationHandle = 0;

// WndProc �Լ��� ApplicationHandle �����Ͱ� ���ԵǾ� �־� Windows �ý����� �޼��� ������ SystemClass�� messagehandler �Լ��� �߰� �� �� �ִ�.

#endif