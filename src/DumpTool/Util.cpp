#include "Util.h"
#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <filesystem>

bool IsDiskPath(const std::wstring& path);
std::wstring GetAccurateFolderPathInternal(const std::wstring& folderPath);

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

std::wstring StringToWstring(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], sizeNeeded);
	return wstr;
}

void ToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

std::string GetLowerString(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}

void ToUpper(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) { return std::toupper(c); });
}

std::vector<std::string> StringSplit(const std::string& text, char separator, bool isRemoveEmpty, bool isRemoveDuplicates)
{
	std::vector<std::string> result;
	std::istringstream stringStream(text);
	std::string str;
	while (std::getline(stringStream, str, separator))
	{
		if (isRemoveEmpty && str.size() == 0)
		{
			continue;
		}
		if (isRemoveDuplicates)
		{
			int i = 0;
			for (; i < result.size(); ++i)
			{
				if (result[i] == str)
				{
					break;
				}
			}
			if (i < result.size())
			{
				continue;
			}
		}
		result.push_back(str);
	}
	return result;
}

std::vector<std::wstring> WstringSplit(const std::wstring& text, wchar_t separator, bool isRemoveEmpty, bool isRemoveDuplicates)
{
	std::vector<std::wstring> result;
	std::wistringstream stringStream(text);
	std::wstring str;
	while (std::getline(stringStream, str, separator))
	{
		if (isRemoveEmpty && str.size() == 0)
		{
			continue;
		}
		if (isRemoveDuplicates)
		{
			int i = 0;
			for (; i < result.size(); ++i)
			{
				if (result[i] == str)
				{
					break;
				}
			}
			if (i < result.size())
			{
				continue;
			}
		}
		result.push_back(str);
	}
	return result;
}

std::wstring GetAccurateFolderPath(const std::wstring& folderPath)
{
	if (folderPath.size() > 0)
	{
		auto lastChar = folderPath[folderPath.size() - 1];
		if (lastChar == '\\' || lastChar == '/')
		{
			return GetAccurateFolderPathInternal(folderPath.substr(0, folderPath.size() - 1));
		}
		else
		{
			return GetAccurateFolderPathInternal(folderPath);
		}
	}
	return std::wstring();
}

std::wstring GetAccurateName(const std::wstring& folderPath, const std::wstring& fileName)
{
	for (const auto& item : std::filesystem::directory_iterator(folderPath))
	{
		if (item.is_regular_file())
		{
			auto name = std::filesystem::path(item).filename().wstring();
			if (_wcsicmp(name.c_str(), fileName.c_str()) == 0)
			{
				return name;
			}
		}
	}
	return std::wstring();
}

std::vector<std::wstring> GetCommand(const char* const charArray, size_t charArrayLength)
{
	std::vector<std::wstring> result;
	if (charArray != nullptr && charArrayLength > 0)
	{
		for (size_t i = 0; i < charArrayLength; ++i)
		{
			if (charArray[i] != ' ')
			{
				std::wstring text;

				bool hasDoubleQuotes = charArray[i] == '\"';
				size_t j = hasDoubleQuotes ? i + 1 : i;
				for (; j < charArrayLength; ++j)
				{
					if (hasDoubleQuotes)
					{
						if (charArray[j] == '\"')
						{
							break;
						}
					}
					else if (charArray[j] == ' ')
					{
						break;
					}
					text += charArray[j];
				}
				i = j;

				if (text.size() > 0)
				{
					result.push_back(text);
				}
			}
		}
	}
	return result;
}

std::vector<std::wstring> GetCommand(std::wstring command)
{
	std::vector<std::wstring> result;
	if (command.size() > 0)
	{
		for (size_t i = 0; i < command.size(); ++i)
		{
			if (command[i] != ' ')
			{
				std::wstring text;

				bool hasDoubleQuotes = command[i] == '\"';
				size_t j = hasDoubleQuotes ? i + 1 : i;
				for (; j < command.size(); ++j)
				{
					if (hasDoubleQuotes)
					{
						if (command[j] == '\"')
						{
							break;
						}
					}
					else if (command[j] == ' ')
					{
						break;
					}
					text += command[j];
				}
				i = j;

				if (text.size() > 0)
				{
					result.push_back(text);
				}
			}
		}
	}
	return result;
}

// 判断该路径是否磁盘路径
static bool IsDiskPath(const std::wstring& path)
{
	if (path.size() == 3)
	{
		wchar_t ch = path[0];
		if ((65 <= ch && ch <= 90) || (97 <= ch && ch <= 122))
		{
			return path[1] == ':' && (path[2] == '/' || path[2] == '\\');
		}
	}
	return false;
}

// 获取准确的文件夹路径
static std::wstring GetAccurateFolderPathInternal(const std::wstring& folderPath)
{
	std::filesystem::path path(folderPath);
	std::wstring parentFolderPath = path.parent_path().wstring();
	bool isDiskPath = IsDiskPath(parentFolderPath);
	if (isDiskPath)
	{
		ToUpper(parentFolderPath);
	}
	else
	{
		parentFolderPath = GetAccurateFolderPathInternal(parentFolderPath);
	}

	std::wstring folderName = path.filename().wstring();
	for (const auto& item : std::filesystem::directory_iterator(parentFolderPath))
	{
		if (item.is_directory())
		{
			auto name = std::filesystem::path(item).filename().wstring();
			if (_wcsicmp(name.c_str(), folderName.c_str()) == 0)
			{
				folderName = name;
				break;
			}
		}
	}

	if (isDiskPath)
	{
		if (parentFolderPath[parentFolderPath.size() - 1] == '/')
		{
			parentFolderPath[parentFolderPath.size() - 1] = '\\';
		}
		return parentFolderPath + folderName;
	}
	return parentFolderPath + L"\\" + folderName;
}
