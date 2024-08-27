#pragma once
#include <string>
#include <vector>
#include <map>

class DumpTool
{
public:
	DumpTool();
	~DumpTool();
	bool GetPlatform(const std::wstring& filePath, std::wstring& platform);
	bool GetPlatforms(const std::wstring& folderPath, std::vector<std::wstring>& x64PlatformList, std::vector<std::wstring>& x86PlatformList, std::vector<std::wstring>& otherPlatformList);
	bool GetExports(const std::wstring& filePath, std::vector<std::wstring>& exportList);
	bool GetDependents(const std::wstring& filePath, std::map<std::wstring, std::vector<std::wstring>>& existDependents, std::vector<std::wstring>& lostDependents);

private:
	bool GetDependents(const std::wstring& path, const std::wstring name, std::map<std::wstring, std::vector<std::wstring>>& existDependents, std::vector<std::wstring>& lostDependents);
	bool GetDllPath(const std::wstring dllName, std::wstring& dllPath);
	bool IsVectorContains(const std::vector<std::wstring>& list, const std::wstring& item);
	bool IsMapContains(const std::map<std::wstring, std::vector<std::wstring>>& dict, const std::wstring& item);
	bool RunCommand(const std::wstring& command, std::wstring& output);

private:
	std::wstring dumpBinPath = L"dumpbin\\dumpbin.exe";
};
