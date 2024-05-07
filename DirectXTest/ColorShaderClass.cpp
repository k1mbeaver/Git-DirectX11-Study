#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	// 클래스 생성자는 클래스의 모든 개인 포인터를 null로 초기화 하기!
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}


ColorShaderClass::~ColorShaderClass()
{

}


bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// 초기화 함수는 셰이더에 대한 초기화 함수를 호출, HLSL 셰이더 파일의 이름을 전달한다.
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;


	// Set the filename of the vertex shader.
	error = wcscpy_s(vsFilename, 128, L"../DirectXTest/Color.vs");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the pixel shader.
	error = wcscpy_s(psFilename, 128, L"../DirectXTest/Color.ps");
	if (error != 0)
	{
		return false;
	}

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// 정점, 픽셀 셰이더 및 관련 객체 종료
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	// SetShaderParameters 함수를 사용해 세이더 내부에 매개변수를 설정, 매개변수가 설정되면 renderShader를 호출하여 HLSL 셰이더를 사용해 녹색 삼각형을 그림
	bool result;

	// 렌더링에 사용할 셰이더 매개변수를 설정
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// 이제 셰이더를 사용하여 준비된 버퍼를 렌더링
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	// 셰이더 파일을 실제로 로드하고 DirectX 및 GPU에서 사용할 수 있도록 만드는 기능
	// 레이아웃 설정과 정점 버퍼 데이터가 GPU의 그래픽 파이프라인에서 어떻게 보이는지 확인할 수 있다
	// 레이아웃은 modelclass.h 파일의 VertexType과 color.vs 파일에 정의된 VertexType과 일치해야한다.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// 사용할 포인터를 null로 초기화하기
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	/*
	* 이 부분에서 셰이더 프로그램을 버퍼로 컴파일한다.
	* 셰이더 파일 이름, 셰이더 이름, 셰이더 버전, 셰이더를 컴파일할 버퍼를 지정합니다. 
	* 셰이더 컴파일에 실패하면 오류를 기록하기 위해 다른 함수로 보내는 errorMessage 문자열 안에 오류 메시지를 넣습니다. 여전히 실패하고 errorMessage 문자열이 없으면 셰이더 파일을 찾을 수 없다는 의미
	* 대화상자가 팝업으로 표시
	*/
	// Vertex Shader 코드를 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// 만약 세이더가 컴파일에 실패했다면 오류 메시지에 무언가를 썼어야합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		// 오류 메시지에 아무 내용도 없으면 단순히 셰이더 파일 자체를 찾을 수 없는 것입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드를 컴파일합니다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	/*
	* 정점 셰이더와 픽셀 셰이더 코드가 성공적으로 버퍼로 컴파일되면 해당 버퍼를 사용해 셰이더 개체 자체를 생성
	* 이 시점부터 정점 및 픽셀 셰이더와 인터페이스하기 위해 이러한 포인터를 사용
	*/

	// 버퍼에서 정점 셰이더를 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼에서 픽셀 셰이더를 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* 셰이더에서 처리할 정점 데이터의 레이아웃을 만드는 것입니다. 이 셰이더는 위치 및 색상 벡터를 사용하므로 두 가지의 크기를 지정하는 레이아웃에서 두 가지를 모두 생성해야합니다.
	* 체계 이름은 레이아웃에서 가장 먼저 작성해야 하는 항목으로, 이를 통해 셰이더는 레이아웃에서 이 요소의 사용법을 결정할 수 있습니다.
	*/
	
	// 정점 입력 레이아웃 설명을 만듭니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야한다.

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 위치 벡터
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0; // 데이터 사이의 간격
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 색상
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // 고유한 값 대신 이것을 사용하면 간격이 자동으로 파악됨
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/* 
	* 레이아웃 설명이 설정되면 크기를 가져온 다음 D3D 장치를 사용해 입력 레이아웃을 생성할 수 있다.
	* 또한 레이아웃이 생성되면 더 이상 필요하지 않으므로 정점 및 픽셀 셰이더 버퍼를 해제합니다.
	*/

	// 레이아웃의 요소 수를 가져옵니다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	// 버퍼는 레이아웃 생성후에는 필요없으므로 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 셰이더를 활용하기 위해 설정해야 하는 마지막은 상수 버퍼
	// 정점 셰이더에서 본 것처럼 현재 상수 버퍼가 하나만 있으므로 여기서 하나만 설정하면 세이더와 인터페이스 할 수 있음
	// 버퍼 사용량은 매 프레임마다 업데이트되므로 동적으로 설정해야한다.
	// 바인드 플래그는 이 버퍼가 상수 버퍼가 될 것임을 나타낸다.
	// CPU 액세스 플래그는 사용법과 일치해야 D3D11_CPU_ACCESS_WRITE로 설정됩니다.
	// 설명을 ㅏㅈㄱ성하고 나면 상수 버퍼 인터페이스를 생성한 다음 이를 사용해 SetShaderParameters 함수를 사용해 셰이더의 내부 변수에 액세스 가능

	// 버텍스 셰이더에 있는 동적 행렬 상수 버퍼의 설명을 설정
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있게 상수 버퍼 포인터를 만든다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// InitializeShader 함수에 설정된 4개의 인터페이스를 해제
	
	// 행렬 상수 버퍼를 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃을 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더를 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점 세이더를 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// 버텍스 셰이더나 픽셀 셰이더를 컴파일할 때 생성되는 오류 메시지 기록
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	//오류 메시지 텍스트 버퍼에 대한 포인터 가져오기
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지의 길이 가져오기
	bufferSize = errorMessage->GetBufferSize();

	// 오류 메시지를 쓸 파일을 연다
	fout.open("shader-error.txt");

	// 오류 메시지를 작성한다.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	// 세이더에서 전역 변수를 더 쉽게 설정할 수 있도록 존재
	// 이 함수에 사용된 행렬은 ApplicationClass 내부에서 생성된 후 Render 함수 호출 중에 정점 셰이더로 보내기 위해 이 함수가 호출된다.

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 행렬을 셰이더로 보내기 전에 행렬을 전치해야 한다. (Dirextx 11 요구사항)
	// 전치행렬 : 임의의 행렬 A가 주어졌을 때, 그 행렬의 행과 열을 바꾸어 얻어낸 행렬을 의미
	
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 상수 버퍼를 잠그고 그 안에 새 행렬을 설정한 다음 잠금을 해제
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬을 상수 버퍼에 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 잠금 해제
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 이제 HLSL 정점 셰이더에서 업데이트 된 행렬 버퍼를 설정
	
	// Vertex Shader에서 상수 버퍼의 위치를 설정
	bufferNumber = 0;

	// 마지막으로 업데이트 된 값으로 정점 셰이더의 상수 버퍼를 설정
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}


void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// RenderShader는 Render 함수에서 호출되는 두 번째 함수, 
	// SetShaderParameters는 셰이더 매개변수가 올바르게 설정되었는지 확인하기 위해 이 전에 호출
	// 이 함수의 첫 번째 단게는 입력 어셈블러에서 입력 레이아웃을 활성화하도록 설정하는 것
	// 이를 통해 GPU는 정점 버퍼의 데이터 형식을 알 수 있음
	// 두 번째 단게는 이 정점 버퍼를 렌더링하는 데 사용할 정점 셰이더와 픽셀 셰이더를 설정하는 것
	// 셰이더가 설정돠면 D3D 장치 컨텍스트를 사용해 DrawIndexed DirectX 11 함수를 호출하여 삼각형을 렌더링
	// 이 함수가 호출 되면 녹색 삼각형이 호출된다.
	
	// 정점 입력 레이아웃 설정
	deviceContext->IASetInputLayout(m_layout);

	// 이 삼각형을 렌더링하는 데 사용할 정점 및 픽셀 세이더를 설정
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 렌더링
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}