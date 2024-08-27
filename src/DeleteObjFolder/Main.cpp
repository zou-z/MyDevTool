#include <cstdio>
#include <filesystem>
#include <Windows.h>

std::wstring CharArrayToWstring(const char* const charArray)
{
	int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, charArray, -1, nullptr, 0);
	if (wideCharLength == 0)
	{
		return L"";
	}
	std::wstring wideString(wideCharLength, 0);
	MultiByteToWideChar(CP_UTF8, 0, charArray, -1, &wideString[0], wideCharLength);
	wideString.resize((size_t)wideCharLength - 1);
	return wideString;
}

static void DeleteObjFolder(const std::wstring& folderPath)
{
	for (const auto& item : std::filesystem::directory_iterator(folderPath))
	{
		if (item.is_directory())
		{
			auto folderName = std::filesystem::path(item).filename().wstring();
			if (folderName[0] == '.')
			{
				continue;
			}

			if (folderName == L"obj")
			{
				auto objFolderPath = std::filesystem::path(item);
				std::error_code code;
				if (std::filesystem::remove_all(objFolderPath, code))
				{
					printf("Delete Success: %ws\n", objFolderPath.wstring().c_str());
				}
				else
				{
					printf("Delete Failed: %ws\n%s\n", objFolderPath.wstring().c_str(), code.message().c_str());
				}
				
				// 一个项目只有一个obj文件夹
				return;
			}
			else
			{
				DeleteObjFolder(std::filesystem::path(item).wstring());
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::wstring folderPath = CharArrayToWstring(argv[1]);
		if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
		{
			DeleteObjFolder(folderPath);
		}
		else
		{
			printf("Invalid Folder Path\n");
		}
	}
	else
	{
		printf("Invalid Command\n");
	}
	return 0;
}
