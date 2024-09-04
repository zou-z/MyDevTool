#pragma once
#include <Windows.h>
#include "qrcodegen.hpp"

class MainWindow
{
public:
	MainWindow(HINSTANCE instance, int cmdShow);
	~MainWindow();
	int Show(qrcodegen::QrCode* qrCode, const std::wstring& text);
	qrcodegen::QrCode* GetQrCode() const;

private:
	bool Init();
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static MainWindow* mainWindow;
	HINSTANCE instance;
	HWND hwnd = nullptr;
	int cmdShow;

	std::wstring text;
	qrcodegen::QrCode* qrCode = nullptr;
	int qrCodeMargin = 24;
	int qrCodeCellSize = 5;
};