#include <cstdio>
#include <locale>
#include <iostream>
#include <filesystem>
#include "Util.h"
#include "DumpTool.h"

DumpTool dumpTool{};

static void OutputHelp(bool isShowLoopCommand = false)
{
	printf("  Get Platform: platform <file path or folder path>\n");
	printf("Get Dependents: dependents <file path>\n");
	printf("   Get Exports: exports <file path>\n");
	if (isShowLoopCommand)
	{
		printf("          Help: help\n");
		printf("          Exit: exit\n");
	}
	printf("\n");
}

static void OutputError()
{
	printf("Invalid Command\n\n");
}

static void OutputPlatform(const std::wstring& path)
{
	if (std::filesystem::exists(path))
	{
		if (std::filesystem::is_regular_file(path))
		{
			std::wstring platform;
			if (dumpTool.GetPlatform(path, platform))
			{
				printf("%ws\n\n", platform.c_str());
			}
			else
			{
				printf("Get Platform Failed\n");
			}
		}
		if (std::filesystem::is_directory(path))
		{
			std::vector<std::wstring> x64PlatformList;
			std::vector<std::wstring> x86PlatformList;
			std::vector<std::wstring> otherPlatformList;
			if (dumpTool.GetPlatforms(path, x64PlatformList, x86PlatformList, otherPlatformList))
			{
				if (x64PlatformList.size() > 0)
				{
					printf("Platform x64:\n");
					for (const auto& item : x64PlatformList)
					{
						printf("    %ws\n", item.c_str());
					}
					printf("\n");
				}
				if (x86PlatformList.size() > 0)
				{
					printf("Platform x86:\n");
					for (const auto& item : x86PlatformList)
					{
						printf("    %ws\n", item.c_str());
					}
					printf("\n");
				}
				if (otherPlatformList.size() > 0)
				{
					printf("Platform other:\n");
					for (const auto& item : otherPlatformList)
					{
						printf("    %ws\n", item.c_str());
					}
					printf("\n");
				}
				if (x64PlatformList.size() == 0 && x86PlatformList.size() == 0 && otherPlatformList.size() == 0)
				{
					printf("(None)\n\n");
				}
			}
			else
			{
				printf("Get Platforms Failed\n");
			}
		}
	}
	else
	{
		OutputHelp();
	}
}

static void OutputExports(const std::wstring& filePath)
{
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		std::vector<std::wstring> exportList;
		if (dumpTool.GetExports(filePath, exportList))
		{
			for (const auto& item : exportList)
			{
				printf("%ws\n", item.c_str());
			}
			if (exportList.size() == 0)
			{
				printf("(None)\n");
			}
			printf("\n");
		}
		else
		{
			printf("Get Exports Failed\n");
		}
	}
	else
	{
		OutputHelp();
	}
}

static void OutputDependents(const std::wstring& filePath)
{
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		std::map<std::wstring, std::vector<std::wstring>> existDependents;
		std::vector<std::wstring> lostDependents;
		if (dumpTool.GetDependents(filePath, existDependents, lostDependents))
		{
			printf("Dependents Exist:\n");
			for (const auto& keyValuePair : existDependents)
			{
				printf("    %ws\n", keyValuePair.first.c_str());
				for (const auto& item : keyValuePair.second)
				{
					printf("        %ws\n", item.c_str());
				}
			}
			if (existDependents.size() == 0)
			{
				printf("    (None)\n");
			}
			printf("\n");

			printf("Dependents Lost:\n");
			for (const auto& item : lostDependents)
			{
				printf("    %ws\n", item.c_str());
			}
			if (lostDependents.size() == 0)
			{
				printf("    (None)\n");
			}
			printf("\n");
		}
		else
		{
			printf("Get Dependents Failed\n");
		}
	}
	else
	{
		OutputHelp();
	}
}

static void RunCommand(const std::wstring& command, const std::wstring& parameter)
{
	if (command == L"platform")
	{
		OutputPlatform(parameter);
	}
	else if (command == L"exports")
	{
		OutputExports(parameter);
	}
	else if (command == L"dependents")
	{
		OutputDependents(parameter);
	}
	else
	{
		OutputError();
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		setlocale(LC_ALL, "chs");
		OutputHelp(true);
		while (true)
		{
			std::wstring input;
			printf("Input> ");
			std::getline(std::wcin, input);
			auto command = GetCommand(input);

			if (command.size() > 0)
			{
				if (command[0] == L"exit")
				{
					break;
				}
				else if (command[0] == L"help")
				{
					OutputHelp(true);
				}
				else
				{
					if (command.size() >= 2)
					{
						RunCommand(command[0], command[1]);
					}
					else
					{
						OutputError();
					}
				}
			}
		}
	}
	else if (argc > 1)
	{
		if (strcmp(argv[1], "help") == 0)
		{
			OutputHelp();
		}
		else if (argc >= 3)
		{
			std::wstring command = CharArrayToWstring(argv[1]);
			std::wstring parameter = CharArrayToWstring(argv[2]);
			RunCommand(command, parameter);
		}
		else
		{
			OutputError();
		}
	}
	return 0;
}
