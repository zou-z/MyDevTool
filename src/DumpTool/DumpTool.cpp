#include "DumpTool.h"
#include <Windows.h>
#include <regex>
#include <filesystem>
#include "Util.h"

DumpTool::DumpTool()
{
}

DumpTool::~DumpTool()
{
}

bool DumpTool::GetPlatform(const std::wstring& filePath, std::wstring& platform)
{
	std::wstring output;
	std::wstring command = dumpBinPath + L" /headers \"" + filePath + L"\"";
	if (RunCommand(command, output))
	{
		std::wsmatch match;
		if (std::regex_search(output, match, std::wregex(L"machine \\((.*?)\\)")))
		{
			platform = match[1].str();
			return true;
		}
	}
	return false;
}

bool DumpTool::GetPlatforms(const std::wstring& folderPath, std::vector<std::wstring>& x64PlatformList, std::vector<std::wstring>& x86PlatformList, std::vector<std::wstring>& otherPlatformList)
{
	// 遍历文件夹
	for (const auto& item : std::filesystem::directory_iterator(folderPath))
	{
		// 判断是否文件类型
		if (item.is_regular_file())
		{
			// 判断文件扩展名
			auto fileName = std::filesystem::path(item).filename();
			if (fileName.has_extension())
			{
				auto extension = GetLowerString(fileName.extension().string());
				if (extension == ".dll" || extension == ".exe")
				{
					std::wstring platform;
					if (!GetPlatform(item.path(), platform))
					{
						x64PlatformList.clear();
						x86PlatformList.clear();
						otherPlatformList.clear();
						return false;
					}

					if (platform == L"x64")
					{
						x64PlatformList.push_back(fileName.wstring());
					}
					else if (platform == L"x86")
					{
						x86PlatformList.push_back(fileName.wstring());
					}
					else
					{
						otherPlatformList.push_back(platform + L" " + fileName.wstring());
					}
				}
			}
		}
	}
	return true;
}

bool DumpTool::GetExports(const std::wstring& filePath, std::vector<std::wstring>& exportList)
{
	std::wstring output;
	std::wstring command = dumpBinPath + L" /exports \"" + filePath + L"\"";
	if (RunCommand(command, output))
	{
		std::wregex pattern(L"[0-9A-F]{8} (.*?) = ");
		std::wsregex_iterator iter(output.begin(), output.end(), pattern);
		std::wsregex_iterator iterEnd;
		for (; iter != iterEnd; ++iter)
		{
			exportList.push_back(iter->str(1));
		}
		return true;
	}
	return false;
}

bool DumpTool::GetDependents(const std::wstring& filePath, std::map<std::wstring, std::vector<std::wstring>>& existDependents, std::vector<std::wstring>& lostDependents)
{
	std::filesystem::path fileSystemPath(filePath);
	std::wstring path = fileSystemPath.parent_path().wstring();
	std::wstring name = fileSystemPath.filename().wstring();
	return GetDependents(path, name, existDependents, lostDependents);
}

bool DumpTool::GetDependents(const std::wstring& path, const std::wstring name, std::map<std::wstring, std::vector<std::wstring>>& existDependents, std::vector<std::wstring>& lostDependents)
{
	std::wstring output;
	std::wstring command = dumpBinPath + L" /dependents \"" + path + L"\\" + name + L"\"";
	if (RunCommand(command, output))
	{
		std::wregex pattern(L" {4}(.*?).dll");
		std::wsregex_iterator iter(output.begin(), output.end(), pattern);
		std::wsregex_iterator iterEnd;
		for (; iter != iterEnd; ++iter)
		{
			std::wstring dependent = iter->str(1) + L".dll";
			std::wstring dependentPath = path + L"\\" + dependent;
			bool isExistInRootPath = false;
			if (std::filesystem::exists(dependentPath) && std::filesystem::is_regular_file(dependentPath))
			{
				dependent = GetAccurateName(path, dependent);
				isExistInRootPath = true;
			}

			// 判断是否已添加该库
			if (IsMapContains(existDependents, dependent))
			{
				continue;
			}
			if (IsVectorContains(lostDependents, dependent))
			{
				continue;
			}

			if (isExistInRootPath)
			{
				// 如果当前目录中存在该依赖库，则添加到存在的依赖库字典中
				existDependents[path].push_back(dependent);

				// 获取该依赖库所依赖的库
				if (!GetDependents(path, dependent, existDependents, lostDependents))
				{
					return false;
				}
			}
			else
			{
				// 获取系统是否存在该依赖库，如果存在且还未添加，则添加到存在的依赖库字典中
				if (GetDllPath(dependent, dependentPath))
				{
					std::filesystem::path result(dependentPath);
					dependentPath = result.parent_path().wstring();
					std::wstring targetName = result.filename().wstring();
					targetName = GetAccurateName(dependentPath, targetName);

					if (!IsMapContains(existDependents, targetName))
					{
						dependentPath = GetAccurateFolderPath(dependentPath);
						existDependents[dependentPath].push_back(targetName);
					}
					continue;
				}

				// 如果不存在，则添加到丢失列表中
				lostDependents.push_back(dependent);
			}
		}
		return true;
	}
	return false;
}

bool DumpTool::GetDllPath(const std::wstring dllName, std::wstring& dllPath)
{
	bool isSuccess = false;
	HMODULE module = LoadLibrary(dllName.c_str());
	if (module != nullptr)
	{
		wchar_t path[MAX_PATH];
		auto length = GetModuleFileName(module, path, MAX_PATH);
		if (length > 0)
		{
			dllPath = std::wstring(path);
			isSuccess = true;
		}
		FreeLibrary(module);
	}
	return isSuccess;
}

bool DumpTool::IsVectorContains(const std::vector<std::wstring>& list, const std::wstring& item)
{
	for (const auto& listItem : list)
	{
		if (item == listItem)
		{
			return true;
		}
	}
	return false;
}

bool DumpTool::IsMapContains(const std::map<std::wstring, std::vector<std::wstring>>& dict, const std::wstring& item)
{
	for (const auto& keyValuePair : dict)
	{
		if (IsVectorContains(keyValuePair.second, item))
		{
			return true;
		}
	}
	return false;
}

bool DumpTool::RunCommand(const std::wstring& command, std::wstring& output)
{
	bool isSuccess = false;
	HANDLE standardOutRead = nullptr;
	HANDLE standardOutWrite = nullptr;
	do
	{
		// 创建管道接收子进程的标准输出
		SECURITY_ATTRIBUTES securityAttributes{};
		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.bInheritHandle = TRUE;  // 允许子进程继承句柄
		securityAttributes.lpSecurityDescriptor = nullptr;
		if (!CreatePipe(&standardOutRead, &standardOutWrite, &securityAttributes, 0))
		{
			break;
		}
		if (!SetHandleInformation(standardOutRead, HANDLE_FLAG_INHERIT, 0))
		{
			break;
		}

		// 设置启动信息
		STARTUPINFO startupInfo{};
		PROCESS_INFORMATION processInfo{};
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.hStdError = standardOutWrite;
		startupInfo.hStdOutput = standardOutWrite;
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;

		// 创建子进程
		if (!CreateProcess(
			nullptr, const_cast<WCHAR*>(command.c_str()),
			nullptr, nullptr,
			TRUE, 0,
			nullptr, nullptr,
			&startupInfo, &processInfo))
		{
			break;
		}

		// 关闭子进程的写句柄，以便父进程可以读取输出
		CloseHandle(standardOutWrite);
		standardOutWrite = nullptr;

		// 读取子进程的输出
		char buffer[4096]{ 0 };
		while (true)
		{
			DWORD count = 0;
			auto result = ReadFile(standardOutRead, buffer, sizeof(buffer) - 1, &count, nullptr);
			if (!result || count == 0)
			{
				break;
			}
			buffer[count] = 0;
			output += CharArrayToWstring(buffer);
		}

		CloseHandle(standardOutRead);
		standardOutRead = nullptr;
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		isSuccess = true;
	} while (false);
	if (standardOutRead != nullptr)
	{
		CloseHandle(standardOutRead);
	}
	if (standardOutWrite != nullptr)
	{
		CloseHandle(standardOutWrite);
	}
	return isSuccess;
}
