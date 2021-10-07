#include "Common/os.h"
#include "Common/assertion.h"

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS

#include <Windows.h>

#endif

namespace ph::os
{

namespace
{

EWindowsVersion get_windows_version_internal()
{
#ifndef PH_OPERATING_SYSTEM_IS_WINDOWS

	return EWindowsVersion::UNKNOWN;

#else
	
	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexw
	// https://stackoverflow.com/questions/32115255/c-how-to-detect-windows-10

	const auto ntdllHandle = GetModuleHandleA("ntdll");
	if(ntdllHandle == NULL)
	{
		// Normally, ntdll.dll should always be loaded by any Windows application
		PH_ASSERT_MSG(false,
			"ntdll.dll is not loaded by the application.");

		return EWindowsVersion::UNKNOWN;
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
		return EWindowsVersion::UNKNOWN;
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
		return EWindowsVersion::UNKNOWN;
	}

#endif
}

}// end anonymous namespace

EWindowsVersion get_windows_version()
{
	static const EWindowsVersion winVersion = get_windows_version_internal();
	return winVersion;
}

}// end namespace ph::os
