#include <Windows.h>
#include "MainWindow.h"
#include "qrcodegen.hpp"

static std::string WstringToString(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (lpCmdLine == nullptr || wcslen(lpCmdLine) == 0)
	{
		return 0;
	}

	std::wstring text(lpCmdLine);
	auto errorCorrectionLevel = qrcodegen::QrCode::Ecc::LOW;
	qrcodegen::QrCode qrCode = qrcodegen::QrCode::encodeText(WstringToString(text).c_str(), errorCorrectionLevel);

	MainWindow mainWindow(hInstance, nCmdShow);
	return mainWindow.Show(&qrCode, text);
}
