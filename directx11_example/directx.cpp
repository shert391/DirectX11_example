#include "directx.h"

/* 
http://d3dbegin.narod.ru/index.htm
http://www.directxtutorial.com/
*/

void Directx::CleanupDevice()
{
	_pconstantbuffer->Release();
	_pindexbuffer->Release();
	_pvertexbuffer->Release();
	_ppixelshader->Release();
	_pinputlayout->Release();
	_pvertexshader->Release();
    _pdevcon->ClearState();
    _pdevise->Release();
    _pswapchain->Release();
    _pdevcon->Release();
    _pbackbuffer->Release();
}

void Directx::Render()
{
    float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; 
    _pdevcon->ClearRenderTargetView(_pbackbuffer, ClearColor);
	_pdevcon->DrawIndexed(18, 0, 0);
    _pswapchain->Present(0, 0);
}

HRESULT Directx::InitD3D(HWND hwnd)
{
    RECT rc{};

    ZeroMemory(&rc, sizeof(RECT));

    GetClientRect(hwnd, &rc);

	_widthWindow = rc.right - rc.left;
	_heightWindow = rc.bottom - rc.top;
    UINT createDeviceFlags = 0;

    DXGI_SWAP_CHAIN_DESC scd{};
	
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;
    scd.BufferDesc.Width = _widthWindow;
    scd.BufferDesc.Height = _heightWindow;

    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &_pswapchain, &_pdevise, NULL, &_pdevcon);

    if (FAILED(result)) return result;

    ID3D11Texture2D* pBackBuffer = NULL;

    result = _pswapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(result)) return result;

    result = _pdevise->CreateRenderTargetView(pBackBuffer, NULL, &_pbackbuffer);

    pBackBuffer->Release();

    if (FAILED(result)) return result;

    _pdevcon->OMSetRenderTargets(1, &_pbackbuffer, NULL);

    D3D11_VIEWPORT vp{};

    ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));

    vp.Width = (FLOAT)_widthWindow;
    vp.Height = (FLOAT)_heightWindow;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    _pdevcon->RSSetViewports(1, &vp);

    return S_OK;
}

HRESULT Directx::InitPipeline()
{
	ID3D10Blob *blobVertexShader, *blobPixelShader;

	HRESULT result = D3DX11CompileFromFile(L"Shaders.hlsl", 0, 0, "VS", "vs_4_0", D3DCOMPILE_DEBUG, 0, 0, &blobVertexShader, 0, 0);

	if (FAILED(result)) return result;

	result = _pdevise->CreateVertexShader(blobVertexShader->GetBufferPointer(), blobVertexShader->GetBufferSize(), NULL, &_pvertexshader);

	if (FAILED(result)) {
		blobVertexShader->Release();
		return result;
	}

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(inputLayout);

	result = _pdevise->CreateInputLayout(inputLayout, numElements, blobVertexShader->GetBufferPointer(), blobVertexShader->GetBufferSize(), &_pinputlayout);

	blobVertexShader->Release();

	if (FAILED(result)) return result;

	result = D3DX11CompileFromFile(L"Shaders.hlsl", 0, 0, "PS", "ps_4_0", D3DCOMPILE_DEBUG, 0, 0, &blobPixelShader, 0, 0);

	if (FAILED(result)) return result;

	result = _pdevise->CreatePixelShader(blobPixelShader->GetBufferPointer(), blobPixelShader->GetBufferSize(), NULL, &_ppixelshader);

	blobPixelShader->Release();

	if (FAILED(result)) return result;

	VERTEX arrayVertex[] = {
		{ XMFLOAT3(1,  -1,  0) /*C*/, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1,  1,  0) /*D*/, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0,  0,  2) /*E*/, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1,  -1,  0) /*B*/, XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1,  1,  0) /*A*/, XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }
	};
	
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX) * 5;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData)); 

	initData.pSysMem = arrayVertex;             

	result = _pdevise->CreateBuffer(&bd, &initData, &_pvertexbuffer);

	if (FAILED(result)) return result;

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	WORD indices[] =
	{
		2,0,1,
		2,1,4,
		3,2,4,
		2,3,0,
		1,0,4,
		0,3,4,
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 18; 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	bd.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	result = _pdevise->CreateBuffer(&bd, &initData, &_pindexbuffer);

	if (FAILED(result)) return result;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);            
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	bd.CPUAccessFlags = 0;
	result = _pdevise->CreateBuffer(&bd, NULL, &_pconstantbuffer);

	if (FAILED(result)) return result;

	_pdevcon->IASetIndexBuffer(_pindexbuffer, DXGI_FORMAT_R16_UINT, 0);
	_pdevcon->IASetVertexBuffers(0, 1, &_pvertexbuffer, &stride, &offset);
	_pdevcon->VSSetConstantBuffers(0, 1, &_pconstantbuffer);
	_pdevcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pdevcon->IASetInputLayout(_pinputlayout);
	_pdevcon->VSSetShader(_pvertexshader, 0, 0);
	_pdevcon->PSSetShader(_ppixelshader, 0, 0);

	return S_OK;
}

void Directx::SetMatrix()
{
	static float t = 0.0f;

	t += (float)XM_PI * 0.0125f;

	static DWORD dwTimeStart = 0;

	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	t = (dwTimeCur - dwTimeStart) / 1000.0f;
	
	_modelMatrix = XMMatrixRotationZ(t);

	ConstantBuffer cb;
	cb.ModelMatrix = XMMatrixTranspose(_modelMatrix);
	cb.ViewMatrix = XMMatrixTranspose(_viewMatrix);
	cb.ProjectionMatrix = XMMatrixTranspose(_projectionMatrix);

	_pdevcon->UpdateSubresource(_pconstantbuffer, 0, NULL, &cb, 0, 0);
}

void Directx::InitMatrix()
{
	ConstantBuffer constanBuffer;
	//
	XMVECTOR Scaling = XMVectorSet(1, 1, 1, 0);
	XMVECTOR RotationQuaternion = XMVectorSet(0, 0, 0, 0);
	XMVECTOR RotationOrigin = XMVectorSet(0, 0, 0, 0);
	XMVECTOR Translation = XMVectorSet(0, 0, 0, 0);

	_modelMatrix = XMMatrixAffineTransformation(Scaling, RotationOrigin, RotationQuaternion, Translation);

	XMVECTOR Eye = XMVectorSet(5, 0, 1, 0);  // Откуда смотрим
	XMVECTOR At = XMVectorSet(1, 0, 1, 0);    // Куда смотрим
	XMVECTOR Up = XMVectorSet(0, 0, 1, 0);    // Направление верха

	_viewMatrix = XMMatrixLookAtLH(Eye, At, Up);

	_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, _widthWindow / (FLOAT)_heightWindow, 0.01f, 100.0f);
}

