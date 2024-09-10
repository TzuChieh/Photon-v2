#include "Common/os.h"
#include "Common/assertion.h"

#if PH_OPERATING_SYSTEM_IS_WINDOWS

#include <stdlib.h>
#include <Windows.h>

#elif PH_OPERATING_SYSTEM_IS_LINUX

#include <stdio.h>
#include <limits.h>

#elif PH_OPERATING_SYSTEM_IS_OSX

#include <sys/sysctl.h>

#endif

#include <new>
#include <array>

namespace ph::os
{

namespace
{

EWindowsVersion get_windows_version_internal()
{
#if !PH_OPERATING_SYSTEM_IS_WINDOWS

	return EWindowsVersion::Unknown;

#else
	
	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexw
	// https://stackoverflow.com/questions/32115255/c-how-to-detect-windows-10

	const auto ntdllHandle = GetModuleHandleA("ntdll");
	if(ntdllHandle == NULL)
	{
		// Normally, ntdll.dll should always be loaded by any Windows application
		PH_ASSERT_MSG(false,
			"ntdll.dll is not loaded by the application.");

		return EWindowsVersion::Unknown;
	}

	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
	*(reinterpret_cast<FARPROC*>(&RtlGetVersion)) = GetProcAddress(ntdllHandle, "RtlGetVersion");
	
	OSVERSIONINFOEXW osInfo;
	if(RtlGetVersion != NULL)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
	}
	else
	{
		return EWindowsVersion::Unknown;
	}

	if(osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0)
	{
		return EWindowsVersion::Windows_10;
	}
	else if(osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 3)
	{
		return EWindowsVersion::Windows_8_1;
	}
	else if(osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 2)
	{
		return EWindowsVersion::Windows_8;
	}
	else if(osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 1)
	{
		return EWindowsVersion::Windows_7;
	}
	else if(osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 0)
	{
		return EWindowsVersion::Windows_Vista;
	}
	else if(osInfo.dwMajorVersion == 5 && (osInfo.dwMinorVersion == 1 || osInfo.dwMinorVersion == 2))
	{
		return EWindowsVersion::Windows_XP;
	}
	else if(osInfo.dwMajorVersion == 5 && osInfo.dwMinorVersion == 0)
	{
		return EWindowsVersion::Windows_2000;
	}
	else
	{
		return EWindowsVersion::Unknown;
	}

#endif
}

/*!
Reference: https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size
Alternative approach: https://softpixel.com/~cwright/programming/simd/cpuid.php

Code adapted from: https://github.com/NickStrupat/CacheLineSize

Following is the license information of the original code for getting cache line size:

MIT License

Copyright (c) 2022 Nick Strupat

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
std::size_t get_L1_cache_line_size_in_bytes_internal()
{
#if PH_OPERATING_SYSTEM_IS_WINDOWS

	size_t lineSize = 0;
	DWORD bufferSize = 0;
	DWORD i = 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = 0;

	GetLogicalProcessorInformation(0, &bufferSize);
	buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);
	GetLogicalProcessorInformation(&buffer[0], &bufferSize);

	for(i = 0; i != bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i)
	{
		if(buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1)
		{
			lineSize = buffer[i].Cache.LineSize;
			break;
		}
	}

	free(buffer);
	return lineSize;

#elif PH_OPERATING_SYSTEM_IS_LINUX

	FILE* p = 0;
	p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
	unsigned int lineSize = 0;
	if(p)
	{
		fscanf(p, "%d", &lineSize);
		fclose(p);
	}
	return lineSize;

#elif PH_OPERATING_SYSTEM_IS_OSX

	size_t lineSize = 0;
	size_t sizeOfLineSize = sizeof(lineSize);
	sysctlbyname("hw.cachelinesize", &lineSize, &sizeOfLineSize, 0, 0);
	return lineSize;

#else

	#warning "Using an estimated L1 cache size due to unsupported platform."

	return std::hardware_constructive_interference_size;

#endif
}

}// end anonymous namespace

EWindowsVersion get_windows_version()
{
	static const EWindowsVersion winVersion = get_windows_version_internal();
	return winVersion;
}

std::size_t get_L1_cache_line_size_in_bytes()
{
	static const std::size_t numBytes = get_L1_cache_line_size_in_bytes_internal();
	return numBytes;
}

std::filesystem::path get_executable_path()
{
#if PH_OPERATING_SYSTEM_IS_WINDOWS
	std::array<wchar_t, MAX_PATH> buffer{};
	const DWORD length = GetModuleFileNameW(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
	return length > 0 ? buffer.data() : L"";
#elif PH_OPERATING_SYSTEM_IS_LINUX
	std::array<char, PATH_MAX> buffer{};
	const ssize_t numBytes = readlink("/proc/self/exe", buffer/data(), buffer.size());
	return numBytes > 0 ? buffer.data() : "";
#else
	return "";
#endif
}

}// end namespace ph::os
