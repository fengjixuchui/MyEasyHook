#ifndef _EASYHOOK_H_
#define _EASYHOOK_H_

#include <windows.h>

// Dll导出符号
#ifdef EASYHOOK_EXPORTS
#define EASYHOOK_API						__declspec(dllexport) __stdcall
#define DRIVER_SHARED_API(type, decl)		EXTERN_C type EASYHOOK_API decl
#else
#ifndef DRIVER
#define EASYHOOK_API					__declspec(dllimport) __stdcall
#define DRIVER_SHARED_API(type, decl)	EXTERN_C type EASYHOOK_API decl
#else
#define EASYHOOK_API					__stdcall
#define DRIVER_SHARED_API(type, decl)	typedef type EASYHOOK_API PROC_##decl; EXTERN_C type EASYHOOK_API decl
#endif
#endif

#define EASYHOOK_NT_API          EXTERN_C NTSTATUS EASYHOOK_API
#define EASYHOOK_BOOL_API        EXTERN_C BOOL EASYHOOK_API

#define MAX_HOOK_COUNT				1024
#define MAX_ACE_COUNT               128
#define MAX_THREAD_COUNT            128
#define MAX_PASSTHRU_SIZE           1024 * 64

#define EASYHOOK_INJECT_DEFAULT				0x00000000
#define EASYHOOK_INJECT_STEALTH				0x10000000 // (experimental)

typedef struct _REMOTE_ENTRY_INFOR_
{
	ULONG           HostProcessPID;
	UCHAR*          UserData;
	ULONG           UserDataSize;
}REMOTE_ENTRY_INFOR, *PREMOTE_ENTRY_INFOR;


// Remote Hook
// EasyHookDll/Thread.c 导出函数
EASYHOOK_NT_API RhInjectLibrary(INT32 TargetProcessID, INT32 WakeUpThreadID, INT32 InjectionOptions, WCHAR* LibraryPath_x86, WCHAR* LibraryPath_x64, 
							    PVOID InPassThruBuffer, INT32 InPassThruSize);

EASYHOOK_NT_API RhIsX64Process(ULONG32 ProcessID, BOOL * bIsx64);

BOOL EASYHOOK_API GetRemoteModuleExportDirectory(HANDLE ProcessHandle, HMODULE ModuleHandle,
	PIMAGE_EXPORT_DIRECTORY RemoteExportDirectory, IMAGE_DOS_HEADER RemoteDosHeader, IMAGE_NT_HEADERS RemoteNtHeaders);

// EasyHookDll/Stealth.c 导出函数
EASYHOOK_NT_API RhCreateStealthRemoteThread(ULONG32 InTargetProcessID, LPTHREAD_START_ROUTINE InRemoteRoutine,
	PVOID InRemoteParameter, PHANDLE OutRemoteThreadHandle);

// Error.c 导出函数
DRIVER_SHARED_API(PWCHAR, RtlGetLastErrorString());
DRIVER_SHARED_API(NTSTATUS, RtlGetLastError());


// Local Hook
typedef struct _LOCAL_HOOK_INFO_* PLOCAL_HOOK_INFO;
typedef struct _HOOK_TRACE_INFO_
{
	PLOCAL_HOOK_INFO Link;
}HOOK_TRACE_INFO, *PHOOK_TRACE_INFO;
typedef PHOOK_TRACE_INFO TRACED_HOOK_HANDLE;

DRIVER_SHARED_API(NTSTATUS, LhInstallHook(PVOID InEntryPoint, PVOID InHookProc, PVOID InCallBack, TRACED_HOOK_HANDLE OutTracedHookHandle));

// EasyHookDll/LocalHook/Acl.c
#ifndef DRIVER
EASYHOOK_NT_API LhSetInclusiveACL(PULONG InThreadIdList, ULONG InThreadCount, TRACED_HOOK_HANDLE InHandle);
EASYHOOK_NT_API LhSetGlobalInclusiveACL(PULONG InThreadIdList, ULONG InThreadCount);
#endif

// EasyHookDll/LocalHook/Uninstall.c
EASYHOOK_NT_API LhuninstallHook(TRACED_HOOK_HANDLE InHandle);
EASYHOOK_NT_API LhWaitForPendingRemovals();

#endif