#pragma once
#include <vector>
#include <string>

// char����תstd::wstring
std::wstring CharArrayToWstring(const char* const charArray);

// std::stringתstd::wstring
std::wstring StringToWstring(const std::string& str);

// std::stringתСд
void ToLower(std::string& str);

// ��ȡstd::string��Сдstd::string
std::string GetLowerString(const std::string& str);

// std::wstringת��д
void ToUpper(std::wstring& str);

// std::string���ַ�����
std::vector<std::string> StringSplit(const std::string& text, char separator, bool isRemoveEmpty = false, bool isRemoveDuplicates = false);

// std::wstring���ַ�����
std::vector<std::wstring> WstringSplit(const std::wstring& text, wchar_t separator, bool isRemoveEmpty = false, bool isRemoveDuplicates = false);

// ��ȡ׼ȷ���ļ���·����Windows�ļ�ϵͳĬ�ϲ����ִ�Сд��
std::wstring GetAccurateFolderPath(const std::wstring& folderPath);

// ��ȡ׼ȷ���ļ�����Windows�ļ�ϵͳĬ�ϲ����ִ�Сд��
std::wstring GetAccurateName(const std::wstring& folderPath, const std::wstring& fileName);

// ���������ȡ����
std::vector<std::wstring> GetCommand(const char* const charArray, size_t charArrayLength);

// ���������ȡ����
std::vector<std::wstring> GetCommand(std::wstring command);
