class Window {
private:
	HWND hwnd;
	HINSTANCE hInstance;

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
public:
	int Load(HINSTANCE hInstance, int nCmdShow);
};
