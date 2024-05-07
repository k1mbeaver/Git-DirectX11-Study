#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	// Ŭ���� �����ڴ� Ŭ������ ��� ���� �����͸� null�� �ʱ�ȭ �ϱ�!
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
	// �ʱ�ȭ �Լ��� ���̴��� ���� �ʱ�ȭ �Լ��� ȣ��, HLSL ���̴� ������ �̸��� �����Ѵ�.
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
	// ����, �ȼ� ���̴� �� ���� ��ü ����
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	// SetShaderParameters �Լ��� ����� ���̴� ���ο� �Ű������� ����, �Ű������� �����Ǹ� renderShader�� ȣ���Ͽ� HLSL ���̴��� ����� ��� �ﰢ���� �׸�
	bool result;

	// �������� ����� ���̴� �Ű������� ����
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// ���� ���̴��� ����Ͽ� �غ�� ���۸� ������
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	// ���̴� ������ ������ �ε��ϰ� DirectX �� GPU���� ����� �� �ֵ��� ����� ���
	// ���̾ƿ� ������ ���� ���� �����Ͱ� GPU�� �׷��� ���������ο��� ��� ���̴��� Ȯ���� �� �ִ�
	// ���̾ƿ��� modelclass.h ������ VertexType�� color.vs ���Ͽ� ���ǵ� VertexType�� ��ġ�ؾ��Ѵ�.

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// ����� �����͸� null�� �ʱ�ȭ�ϱ�
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	/*
	* �� �κп��� ���̴� ���α׷��� ���۷� �������Ѵ�.
	* ���̴� ���� �̸�, ���̴� �̸�, ���̴� ����, ���̴��� �������� ���۸� �����մϴ�. 
	* ���̴� �����Ͽ� �����ϸ� ������ ����ϱ� ���� �ٸ� �Լ��� ������ errorMessage ���ڿ� �ȿ� ���� �޽����� �ֽ��ϴ�. ������ �����ϰ� errorMessage ���ڿ��� ������ ���̴� ������ ã�� �� ���ٴ� �ǹ�
	* ��ȭ���ڰ� �˾����� ǥ��
	*/
	// Vertex Shader �ڵ带 ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// ���� ���̴��� �����Ͽ� �����ߴٸ� ���� �޽����� ���𰡸� �����մϴ�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		// ���� �޽����� �ƹ� ���뵵 ������ �ܼ��� ���̴� ���� ��ü�� ã�� �� ���� ���Դϴ�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������մϴ�.
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
	* ���� ���̴��� �ȼ� ���̴� �ڵ尡 ���������� ���۷� �����ϵǸ� �ش� ���۸� ����� ���̴� ��ü ��ü�� ����
	* �� �������� ���� �� �ȼ� ���̴��� �������̽��ϱ� ���� �̷��� �����͸� ���
	*/

	// ���ۿ��� ���� ���̴��� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	* ���̴����� ó���� ���� �������� ���̾ƿ��� ����� ���Դϴ�. �� ���̴��� ��ġ �� ���� ���͸� ����ϹǷ� �� ������ ũ�⸦ �����ϴ� ���̾ƿ����� �� ������ ��� �����ؾ��մϴ�.
	* ü�� �̸��� ���̾ƿ����� ���� ���� �ۼ��ؾ� �ϴ� �׸�����, �̸� ���� ���̴��� ���̾ƿ����� �� ����� ������ ������ �� �ֽ��ϴ�.
	*/
	
	// ���� �Է� ���̾ƿ� ������ ����ϴ�.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ��Ѵ�.

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // ��ġ ����
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0; // ������ ������ ����
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // ����
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // ������ �� ��� �̰��� ����ϸ� ������ �ڵ����� �ľǵ�
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/* 
	* ���̾ƿ� ������ �����Ǹ� ũ�⸦ ������ ���� D3D ��ġ�� ����� �Է� ���̾ƿ��� ������ �� �ִ�.
	* ���� ���̾ƿ��� �����Ǹ� �� �̻� �ʿ����� �����Ƿ� ���� �� �ȼ� ���̴� ���۸� �����մϴ�.
	*/

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	// ���۴� ���̾ƿ� �����Ŀ��� �ʿ�����Ƿ� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���̴��� Ȱ���ϱ� ���� �����ؾ� �ϴ� �������� ��� ����
	// ���� ���̴����� �� ��ó�� ���� ��� ���۰� �ϳ��� �����Ƿ� ���⼭ �ϳ��� �����ϸ� ���̴��� �������̽� �� �� ����
	// ���� ��뷮�� �� �����Ӹ��� ������Ʈ�ǹǷ� �������� �����ؾ��Ѵ�.
	// ���ε� �÷��״� �� ���۰� ��� ���۰� �� ������ ��Ÿ����.
	// CPU �׼��� �÷��״� ������ ��ġ�ؾ� D3D11_CPU_ACCESS_WRITE�� �����˴ϴ�.
	// ������ ���������ϰ� ���� ��� ���� �������̽��� ������ ���� �̸� ����� SetShaderParameters �Լ��� ����� ���̴��� ���� ������ �׼��� ����

	// ���ؽ� ���̴��� �ִ� ���� ��� ��� ������ ������ ����
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� �׼��� �� �� �ְ� ��� ���� �����͸� �����.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// InitializeShader �Լ��� ������ 4���� �������̽��� ����
	
	// ��� ��� ���۸� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// ���̾ƿ��� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���� ���̴��� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// ���ؽ� ���̴��� �ȼ� ���̴��� �������� �� �����Ǵ� ���� �޽��� ���
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	//���� �޽��� �ؽ�Ʈ ���ۿ� ���� ������ ��������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽����� ���� ��������
	bufferSize = errorMessage->GetBufferSize();

	// ���� �޽����� �� ������ ����
	fout.open("shader-error.txt");

	// ���� �޽����� �ۼ��Ѵ�.
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
	// ���̴����� ���� ������ �� ���� ������ �� �ֵ��� ����
	// �� �Լ��� ���� ����� ApplicationClass ���ο��� ������ �� Render �Լ� ȣ�� �߿� ���� ���̴��� ������ ���� �� �Լ��� ȣ��ȴ�.

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ����� ���̴��� ������ ���� ����� ��ġ�ؾ� �Ѵ�. (Dirextx 11 �䱸����)
	// ��ġ��� : ������ ��� A�� �־����� ��, �� ����� ��� ���� �ٲپ� �� ����� �ǹ�
	
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ���۸� ��װ� �� �ȿ� �� ����� ������ ���� ����� ����
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ����� ��� ���ۿ� ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ����
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� HLSL ���� ���̴����� ������Ʈ �� ��� ���۸� ����
	
	// Vertex Shader���� ��� ������ ��ġ�� ����
	bufferNumber = 0;

	// ���������� ������Ʈ �� ������ ���� ���̴��� ��� ���۸� ����
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}


void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// RenderShader�� Render �Լ����� ȣ��Ǵ� �� ��° �Լ�, 
	// SetShaderParameters�� ���̴� �Ű������� �ùٸ��� �����Ǿ����� Ȯ���ϱ� ���� �� ���� ȣ��
	// �� �Լ��� ù ��° �ܰԴ� �Է� ��������� �Է� ���̾ƿ��� Ȱ��ȭ�ϵ��� �����ϴ� ��
	// �̸� ���� GPU�� ���� ������ ������ ������ �� �� ����
	// �� ��° �ܰԴ� �� ���� ���۸� �������ϴ� �� ����� ���� ���̴��� �ȼ� ���̴��� �����ϴ� ��
	// ���̴��� �����¸� D3D ��ġ ���ؽ�Ʈ�� ����� DrawIndexed DirectX 11 �Լ��� ȣ���Ͽ� �ﰢ���� ������
	// �� �Լ��� ȣ�� �Ǹ� ��� �ﰢ���� ȣ��ȴ�.
	
	// ���� �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	// �� �ﰢ���� �������ϴ� �� ����� ���� �� �ȼ� ���̴��� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� ������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}