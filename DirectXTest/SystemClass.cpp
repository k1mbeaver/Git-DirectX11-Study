#include "SystemClass.h"

// 클래스 생성자는 포인터 객체를 null로 초기화한다.
// 이러한 객체는 초기화가 실패했을 때 지워야 하기 때문에 굉장히 중요한 과정이다.
// 만약 개체가 null이 아니면 지울 대상으로 가정한다.
// 그래서 애플리케이션에서 모든포인터와 변수를 null로 초기화하는 것이 좋다.
// 어떤 릴리즈 빌드는 그렇게 하지 않으면 실패하기도 한다.
SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

// 빈 복사 생성자와 소멸자를 만든다.
// 만약 만들지 않으면 일부 컴파일러는 이를 자동으로 생성하므로 비워서 만드는게 좋다.
// 또한 소멸자에서는 객체를 파괴하지 않는다.
// 소멸자가 불리지 않을 수도 있기 때문이다.
// ExitThread()와 같은 특정한 윈도 함수는 클래스 소멸자를 부르지 않아 memory leak을 일으키는 것으로 알려져 있다.
// 물론 더 안전한 버전의 함수를 호출할 수도 있지만, 윈도 운영체제에서는 더욱 조심해서 짜려고 한다.
SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}
// 다음 Initialize 함수는 어플리케이션의 모든 설정을 수행한다.
// 먼저 어플리케이션에서 사용할 창을 생성하는 InitializeWindows를 호출한다.
// 또한 Input 과 Graphics 객체를 초기화한다.
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	m_Input->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

// Shutdown 함수는 Clean Up을 수행한다.
// Input과 Graphics에 관련된 모든것을 종료하고 release한다.
// 창과 그와 관련된 handle도 모두 닫는다.
void SystemClass::Shutdown()
{
	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

// Run 함수는 어플리케이션의 루프와 종료 전까지의 모든 프로세스가 실행되는 곳이다.
// 어플리케이션의 프로세스는 loop라고 불리는 프레임 함수에서 처리된다.
// 나머지 코드는 이것을 염두에 두고 작성될 것이므로 반드시 이해해야 하는 개념이다.
// 수도 코드는 다음과 같다
/*
while not done
	check for windows system messages
	process system messages
	process application loop
	check if user wanted to quit during the frame processing
*/
void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}
// 다음의 Frame 함수는 어플리케이션의 모든 프로세스가 완료되는 곳이다.
// 체크하는건 꽤 단순한데, 사용자가 esc를 눌러 종료하길 원하는지 확인하면 된다.
// 만약 종료를 원하지 않을 땐 Graphics 객체를 호출해 해당 프레임을 처리하여 렌더링하도록 한다.
// 어플리케이션이 확장되면서 우리는 여기에 더 많은 코드를 삽입할 것이다.
bool SystemClass::Frame()
{
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}
// MessageHandler 함수는 창의 시스템 메세지가 바로 전달되는 곳이다.
// 이렇게 하면 우리가 관심있는 특정한 정보들에 대해 관찰할 수 있다.
// 현재 우리는 키가 눌렸는지 떼어졌는지를 읽어서 Input 객체에 전달할 것이다.
// 모든 다른 정보들은 윈도우 기본 메세지 핸들러로 다시 전달된다.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}
// InitializeWindows 함수는 우리가 렌더링할 창을 만드는 코드를 넣는 곳이다.
// screenWidth와 screenHeight을 반환하여 모든 애플리케이션 전반에 사용할 수 있도록 한다.
// 기본 설정으로 창을 생성하여 보더가 없는 단순한 검정 창을 초기화 할 것이다.
// 이 함수는 FULL_SCREEN이라는 전역 변수에 따라 창 모드 혹은 전체화면 모드를 만들어 낸다.
// 이 값이 true이면 전체 데스크탑 화면을 모두 가리게끔 만들어 질 것이다.
// 이 값이 false이면 화면 중앙에 800x600의 창을 만든다.
// GraphicsClass.h 최상단에 이 전역 변수를 수정할 수 있도록 작성해 두었다.
// 이 파일의 헤더가 아닌 그 파일의 전역 변수로 선언한 이유를 나중에 이해할 수 있다.
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}
// 그냥 말그대로 창을 끈다.
// 스크린 설정을 보통으로 돌리고 창과 핸들을 release 한다.
void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

// WndProc 함수는 창이 메세지를 보내는 곳이다.
// 창을 초기화할때 상단의 InitializeWindows 함수에서 wc.IpfnWndProc = WndProc로 창의 이름을 전달한다.
// SystemClass내에 정의된 MessageHandler 함수로 모든 메세지를 바로 전달함으로써 코드를 꺠끗하게 유지시킬 수 있다.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}