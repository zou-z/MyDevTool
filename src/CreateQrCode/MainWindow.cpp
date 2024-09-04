#include "MainWindow.h"
#include <string>

MainWindow* MainWindow::mainWindow = nullptr;
LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

MainWindow::MainWindow(HINSTANCE instance, int cmdShow)
{
	this->instance = instance;
	this->cmdShow = cmdShow;
}

MainWindow::~MainWindow()
{
}

int MainWindow::Show(qrcodegen::QrCode* qrCode, const std::wstring& text)
{
	this->qrCode = qrCode;
	this->text = text;	
	if (!Init())
	{
		return -1;
	}

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

qrcodegen::QrCode* MainWindow::GetQrCode() const
{
	return qrCode;
}

bool MainWindow::Init()
{
	std::wstring className = L"QrCode";
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = instance;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszClassName = className.c_str();
	if (RegisterClassExW(&windowClass) == 0)
	{
		return false;
	}

	int windowTitleBarHeight = 32;
	int qrCodeSize = qrCode->getSize();
	int width = qrCodeMargin * 2 + qrCodeSize * qrCodeCellSize;
	int height = width + windowTitleBarHeight;
	hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		className.c_str(), (text.size() > 50 ? text.substr(0, 50) : text).c_str(), WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, width + 16, height + 7, nullptr, nullptr, instance, this);
	if (hwnd == nullptr)
	{
		return false;
	}

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	return true;
}

LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		MainWindow::mainWindow = static_cast<MainWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		int qrCodeMargin = MainWindow::mainWindow->qrCodeMargin;
		int qrCodeCellSize = MainWindow::mainWindow->qrCodeCellSize;
		qrcodegen::QrCode* qrCode = MainWindow::mainWindow->qrCode;
		int qrCodeSize = qrCode->getSize();
		for (int y = 0; y < qrCodeSize; ++y)
		{
			for (int x = 0; x < qrCodeSize; ++x)
			{
				if (qrCode->getModule(x, y))
				{
					RECT rect{};
					rect.left = qrCodeMargin + x * qrCodeCellSize;
					rect.top = qrCodeMargin + y * qrCodeCellSize;
					rect.right = rect.left + qrCodeCellSize;
					rect.bottom = rect.top + qrCodeCellSize;
					FillRect(hdc, &rect, brush);
				}
			}
		}

		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
