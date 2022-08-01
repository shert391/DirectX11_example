#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

struct VERTEX {
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

struct ConstantBuffer {
	XMMATRIX ModelMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
};

class Directx {
private:
	ID3D11RenderTargetView* _pbackbuffer;
	ID3D11DeviceContext* _pdevcon;
	IDXGISwapChain* _pswapchain;
	ID3D11Device* _pdevise;
	ID3D11VertexShader* _pvertexshader;
	ID3D11InputLayout* _pinputlayout;
	ID3D11PixelShader* _ppixelshader;
	ID3D11Buffer* _pvertexbuffer;
	ID3D11Buffer* _pindexbuffer;
	ID3D11Buffer* _pconstantbuffer;

	UINT _widthWindow;
	UINT _heightWindow;

	XMMATRIX _modelMatrix;
	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;
public:
	void CleanupDevice();
	void Render();
	void SetMatrix();
	void InitMatrix();
	
	HRESULT InitD3D(HWND hwnd);
	HRESULT InitPipeline();
};