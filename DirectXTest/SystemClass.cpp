#include "SystemClass.h"

// Ŭ���� �����ڴ� ������ ��ü�� null�� �ʱ�ȭ�Ѵ�.
// �̷��� ��ü�� �ʱ�ȭ�� �������� �� ������ �ϱ� ������ ������ �߿��� �����̴�.
// ���� ��ü�� null�� �ƴϸ� ���� ������� �����Ѵ�.
// �׷��� ���ø����̼ǿ��� ��������Ϳ� ������ null�� �ʱ�ȭ�ϴ� ���� ����.
// � ������ ����� �׷��� ���� ������ �����ϱ⵵ �Ѵ�.
SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

// �� ���� �����ڿ� �Ҹ��ڸ� �����.
// ���� ������ ������ �Ϻ� �����Ϸ��� �̸� �ڵ����� �����ϹǷ� ����� ����°� ����.
// ���� �Ҹ��ڿ����� ��ü�� �ı����� �ʴ´�.
// �Ҹ��ڰ� �Ҹ��� ���� ���� �ֱ� �����̴�.
// ExitThread()�� ���� Ư���� ���� �Լ��� Ŭ���� �Ҹ��ڸ� �θ��� �ʾ� memory leak�� ����Ű�� ������ �˷��� �ִ�.
// ���� �� ������ ������ �Լ��� ȣ���� ���� ������, ���� �ü�������� ���� �����ؼ� ¥���� �Ѵ�.
SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}
// ���� Initialize �Լ��� ���ø����̼��� ��� ������ �����Ѵ�.
// ���� ���ø����̼ǿ��� ����� â�� �����ϴ� InitializeWindows�� ȣ���Ѵ�.
// ���� Input �� Graphics ��ü�� �ʱ�ȭ�Ѵ�.
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

// Shutdown �Լ��� Clean Up�� �����Ѵ�.
// Input�� Graphics�� ���õ� ������ �����ϰ� release�Ѵ�.
// â�� �׿� ���õ� handle�� ��� �ݴ´�.
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

// Run �Լ��� ���ø����̼��� ������ ���� �������� ��� ���μ����� ����Ǵ� ���̴�.
// ���ø����̼��� ���μ����� loop��� �Ҹ��� ������ �Լ����� ó���ȴ�.
// ������ �ڵ�� �̰��� ���ο� �ΰ� �ۼ��� ���̹Ƿ� �ݵ�� �����ؾ� �ϴ� �����̴�.
// ���� �ڵ�� ������ ����
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
// ������ Frame �Լ��� ���ø����̼��� ��� ���μ����� �Ϸ�Ǵ� ���̴�.
// üũ�ϴ°� �� �ܼ��ѵ�, ����ڰ� esc�� ���� �����ϱ� ���ϴ��� Ȯ���ϸ� �ȴ�.
// ���� ���Ḧ ������ ���� �� Graphics ��ü�� ȣ���� �ش� �������� ó���Ͽ� �������ϵ��� �Ѵ�.
// ���ø����̼��� Ȯ��Ǹ鼭 �츮�� ���⿡ �� ���� �ڵ带 ������ ���̴�.
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
// MessageHandler �Լ��� â�� �ý��� �޼����� �ٷ� ���޵Ǵ� ���̴�.
// �̷��� �ϸ� �츮�� �����ִ� Ư���� �����鿡 ���� ������ �� �ִ�.
// ���� �츮�� Ű�� ���ȴ��� ������������ �о Input ��ü�� ������ ���̴�.
// ��� �ٸ� �������� ������ �⺻ �޼��� �ڵ鷯�� �ٽ� ���޵ȴ�.
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
// InitializeWindows �Լ��� �츮�� �������� â�� ����� �ڵ带 �ִ� ���̴�.
// screenWidth�� screenHeight�� ��ȯ�Ͽ� ��� ���ø����̼� ���ݿ� ����� �� �ֵ��� �Ѵ�.
// �⺻ �������� â�� �����Ͽ� ������ ���� �ܼ��� ���� â�� �ʱ�ȭ �� ���̴�.
// �� �Լ��� FULL_SCREEN�̶�� ���� ������ ���� â ��� Ȥ�� ��üȭ�� ��带 ����� ����.
// �� ���� true�̸� ��ü ����ũž ȭ���� ��� �����Բ� ����� �� ���̴�.
// �� ���� false�̸� ȭ�� �߾ӿ� 800x600�� â�� �����.
// GraphicsClass.h �ֻ�ܿ� �� ���� ������ ������ �� �ֵ��� �ۼ��� �ξ���.
// �� ������ ����� �ƴ� �� ������ ���� ������ ������ ������ ���߿� ������ �� �ִ�.
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
// �׳� ���״�� â�� ����.
// ��ũ�� ������ �������� ������ â�� �ڵ��� release �Ѵ�.
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

// WndProc �Լ��� â�� �޼����� ������ ���̴�.
// â�� �ʱ�ȭ�Ҷ� ����� InitializeWindows �Լ����� wc.IpfnWndProc = WndProc�� â�� �̸��� �����Ѵ�.
// SystemClass���� ���ǵ� MessageHandler �Լ��� ��� �޼����� �ٷ� ���������ν� �ڵ带 �Ʋ��ϰ� ������ų �� �ִ�.
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