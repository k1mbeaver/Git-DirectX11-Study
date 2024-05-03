#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// 여기서 WIN32_LEAN_AND_MEAN을 정의한다.
// 이는 빌드 속도를 높일 수 있는데, 덜 쓰이는 API들을 제거함으로써 win32 헤더 파일의 크기를 감소시킨다.
///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

// SystemClass를 사용할 수 있도록 여기서 프레임워크의 다른 두 클래스를 인클루드한다.
//////////////
// INCLUDES //
//////////////
#include <windows.h>
//Windows.h가 인클루드됨으로써 우리는 window를 생성/파괴할 수 있고 다른 유용한 win32 함수들을 사용할 수 있다.

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "InputClass.h"
#include "GraphicsClass.h"
// class의 정의가 굉장히 단순하다.
// WinMain에서 호출되는 Initialize, ShutDown, Run 함수를 확인할 수 있다.
// 그리고 앞서 소개했던 m_Input과 m_Graphics 객체를 가리키는 포인터가 private 멤버로 선언된 것을 확인할 수 있다.

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
// 함수 PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// 전역변수 //
/////////////
static SystemClass* ApplicationHandle = 0;

// WndProc 함수와 ApplicationHandle 포인터가 포함되어 있어 Windows 시스템의 메세지 루프를 SystemClass의 messagehandler 함수로 중계 할 수 있다.

#endif