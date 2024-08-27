#pragma once
#include <vector>
#include <string>

// char数组转std::wstring
std::wstring CharArrayToWstring(const char* const charArray);

// std::string转std::wstring
std::wstring StringToWstring(const std::string& str);

// std::string转小写
void ToLower(std::string& str);

// 获取std::string的小写std::string
std::string GetLowerString(const std::string& str);

// std::wstring转大写
void ToUpper(std::wstring& str);

// std::string按字符分裂
std::vector<std::string> StringSplit(const std::string& text, char separator, bool isRemoveEmpty = false, bool isRemoveDuplicates = false);

// std::wstring按字符分裂
std::vector<std::wstring> WstringSplit(const std::wstring& text, wchar_t separator, bool isRemoveEmpty = false, bool isRemoveDuplicates = false);

// 获取准确的文件夹路径（Windows文件系统默认不区分大小写）
std::wstring GetAccurateFolderPath(const std::wstring& folderPath);

// 获取准确的文件名（Windows文件系统默认不区分大小写）
std::wstring GetAccurateName(const std::wstring& folderPath, const std::wstring& fileName);

// 解析输入获取命令
std::vector<std::wstring> GetCommand(const char* const charArray, size_t charArrayLength);

// 解析输入获取命令
std::vector<std::wstring> GetCommand(std::wstring command);
