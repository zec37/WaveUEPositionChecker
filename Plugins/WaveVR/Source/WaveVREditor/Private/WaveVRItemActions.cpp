// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRItemActions.h"
#include "Interfaces/IPluginManager.h"
#include <SocketSubsystem.h>
#include <IPAddress.h>
#include "HAL/PlatformFilemanager.h"
#include "HAL/IConsoleManager.h"
#include "GenericPlatform/GenericPlatformFile.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "TlHelp32.h"
#include <Async/Async.h>

DEFINE_LOG_CATEGORY_STATIC(WVRMenuItemAction, Display, All);

bool IsProcessRunning(const FString& ProcessName)
{
	// Convert FString to TCHAR (null-terminated array of TCHAR)
	const TCHAR* ProcessNameTChar = *ProcessName;

	// Get a handle to the specified process
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		// Failed to get the process snapshot
		return false;
	}

	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process in the snapshot
	if (!Process32First(hProcessSnap, &ProcessEntry))
	{
		// Failed to retrieve process information
		CloseHandle(hProcessSnap);
		return false;
	}

	// Iterate through processes in the snapshot to find the specified process
	do
	{
		// Compare process names (case-insensitive)
		if (_tcsicmp(ProcessEntry.szExeFile, ProcessNameTChar) == 0)
		{
			// Process found, close the process snapshot handle and return true
			CloseHandle(hProcessSnap);
			return true;
		}
	} while (Process32Next(hProcessSnap, &ProcessEntry));

	// Process not found, close the process snapshot handle and return false
	CloseHandle(hProcessSnap);
	return false;
}

void KillProcess(const FString& ProcessName)
{
	if (IsProcessRunning(ProcessName))
	{
		FString CommandName = FString(TEXT("/c taskkill /F /IM ")) + ProcessName;
		FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *CommandName, false, true, false, nullptr, 0, nullptr, nullptr);
		UE_LOG(WVRMenuItemAction, Display, TEXT("Kill process %s by command: %s"), *ProcessName, *CommandName);
	}
}

void UWaveVRItemActions::StartDPServer() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("Press Start Streaming Server"));

	// Create a lambda function as a coroutine and use Async to prevent from blocking main thread due to Sleep.
	auto Coroutine = [=]()
	{
		const FString DPServerName = TEXT("RRserver.exe");

		KillProcess(DPServerName);         // Restart DP Server
		KillProcess(TEXT("RRConsole.exe"));// VBS
		KillProcess(TEXT("VHConsole.exe"));// VS (VIVE HUB)

		// Take time to kill process before restart a new DP Server.
		if (IsProcessRunning(DPServerName))
		{
			FPlatformProcess::Sleep(0.5f);
		}

		//UE_LOG(WVRMenuItemAction, Display, TEXT("Working directory is %s"), *(FPlatformProcess::GetCurrentWorkingDirectory()));
		FString workingDir = IPluginManager::Get().FindPlugin("WaveVR")->GetBaseDir();
		FString driveStr = workingDir.Left(2);
		UE_LOG(WVRMenuItemAction, Display, TEXT("Plugin directory is %s, and the drive is %s"), *workingDir, *driveStr);
		FString execCmd;
		if (driveStr.Equals(".."))
		{
			execCmd = TEXT("/c cd ") + workingDir + TEXT("/Prebuilt/DirectPreview/RRServer") + TEXT(" && RRserver");
		}
		else
		{
			execCmd = TEXT("/c ") + driveStr + TEXT(" && cd ") + workingDir + TEXT("/Prebuilt/DirectPreview/RRServer") + TEXT(" && RRserver");
		}
		UE_LOG(WVRMenuItemAction, Display, TEXT("start RRserver by command: %s"), *execCmd);
		FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *execCmd, false, true, false, nullptr, 0, nullptr, nullptr);
	};

	// Execute the coroutine asynchronously using Async
	Async(EAsyncExecution::ThreadPool, [Coroutine]()
	{
		Coroutine();
	});
}

void UWaveVRItemActions::StopDPServer() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("Press Stop Streaming Server"));
	KillProcess(TEXT("RRserver.exe")); // DP Server
	KillProcess(TEXT("RRConsole.exe"));// VBS
	KillProcess(TEXT("VHConsole.exe"));// VS (VIVE HUB)
}

void UWaveVRItemActions::InstallDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("Press Install Device APK"));

	// Create a lambda function as a coroutine and use Async to prevent from blocking main thread due to Sleep.
	auto Coroutine = [=]()
	{
		FString pluginDir = IPluginManager::Get().FindPlugin("WaveVR")->GetBaseDir();
		FString configDir = pluginDir + TEXT("/Prebuilt/DirectPreview/RRClient/");

		FProcHandle pMakeDir = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell mkdir /sdcard/DirectPreview/"), false, true, false, nullptr, 0, nullptr, nullptr);

		FPlatformProcess::Sleep(0.3f);

		FString uninstallCmd = TEXT("/c adb uninstall com.htc.vive.vstreaming");
		UE_LOG(WVRMenuItemAction, Display, TEXT("uninstall apk: %s"), *uninstallCmd);
		FProcHandle punInstall = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *uninstallCmd, false, true, false, nullptr, 0, nullptr, nullptr);

		FPlatformProcess::Sleep(0.3f);

		FString driveStr = configDir.Left(2);

		FString installCmd;
		if (driveStr.Equals(".."))
		{
			installCmd = TEXT("/c cd ") + configDir + TEXT(" && adb install -r -g -d Vive_rrClient.apk");
		}
		else {
			installCmd = TEXT("/c ") + driveStr + TEXT(" && cd ") + configDir + TEXT(" && adb install -r -g -d Vive_rrClient.apk");
		}
		UE_LOG(WVRMenuItemAction, Display, TEXT("install apk: %s"), *installCmd);
		FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *installCmd, false, true, false, nullptr, 0, nullptr, nullptr);
	};

	// Execute the coroutine asynchronously using Async
	Async(EAsyncExecution::ThreadPool, [Coroutine]()
	{
		Coroutine();
	});
}

void UWaveVRItemActions::StartDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("Press Start VIVE Business"));
	FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am start -n com.htc.vive.vstreaming/com.htc.vive.rrclient.RRClient"), false, true, false, nullptr, 0, nullptr, nullptr);
}

void UWaveVRItemActions::StopDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("Press Stop VIVE Business"));

	auto Coroutine = [=]()
	{
		StopDirectPreviewProcessInner();
	};
	// Execute the coroutine asynchronously using Async
	Async(EAsyncExecution::ThreadPool, [Coroutine]()
	{
		Coroutine();
	});
}

void UWaveVRItemActions::StopDirectPreviewProcessInner() {
	FProcHandle pStopAPK = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am force-stop com.htc.vive.vstreaming"), false, true, false, nullptr, 0, nullptr, nullptr);
	FPlatformProcess::Sleep(0.3f);
	FProcHandle pKillProcess = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am kill com.htc.vive.vstreaming"), false, true, false, nullptr, 0, nullptr, nullptr);
}

FString UWaveVRItemActions::GetLocalIPAddr() {
	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
	return localIp->IsValid() ? localIp->ToString(false) : TEXT("");
}

void UWaveVRItemActions::WriteConfig(FString configFile) {
	UE_LOG(WVRMenuItemAction, Display, TEXT("WriteConfig"));
	CheckConfigAndDelete(configFile);

	FString localIP = GetLocalIPAddr();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*configFile);

	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.RenderDevice"));
	int32 Value = 0;
	if (CVar) {
		Value = CVar->GetInt();
		UE_LOG(WVRMenuItemAction, Display, TEXT("Target device: %d"), Value);
	}
	else {
		UE_LOG(WVRMenuItemAction, Display, TEXT("Target device: default FOCUS_3"));
	}

	if (FileHandle) {
		if (Value == 1) { // FOCUS
			UE_LOG(WVRMenuItemAction, Display, TEXT("Make up FOCUS or FOCUS_PLUS config"));
			FString cn = FString(
				TEXT("{\n")
				TEXT(" \"IP\" : \"") + localIP + TEXT("\", \n")
				TEXT(" \"Port\" : 6555,\n")
				TEXT(" \"HMD\" : \"FOCUS\",\n")
				TEXT(" \n")
				TEXT(" \"RenderWidth\" : 1440,\n")
				TEXT(" \"RenderHeight\" : 1600,\n")
				TEXT(" \"RenderSizeScale\" : 1.0,\n")
				TEXT(" \"RenderOverfillScale\" : 1.3,\n")
				TEXT(" \n")
				TEXT(" \"UseAutoPrecdictTime\" : true,\n")
				TEXT(" \"CtlPredictRate\" : 6,\n")
				TEXT(" \"HmdPredictRatio\" : 0.615,\n")
				TEXT(" \"CtlPredictRatio\" : 0.615,\n")
				TEXT(" \"HmdPredict\" : 41,\n")
				TEXT(" \"ControllerPredict\" : 40,\n")
				TEXT(" \"MaxHmdPredictTimeInMs\" : 35,\n")
				TEXT(" \"MaxCtlPredictTimeInMs\" : 20,\n")
				TEXT(" \n")
				TEXT(" \"RoomHeight\" : 1.6\n")
				TEXT("}\n")
			);
			FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*cn), cn.Len());
			delete FileHandle;
		}
		else {
			UE_LOG(WVRMenuItemAction, Display, TEXT("Make up FOCUS_3 config"));
			FString cn = FString(
				TEXT("{\n")
				TEXT(" \"IP\" : \"") + localIP + TEXT("\", \n")
				TEXT(" \"Port\" : 6555,\n")
				TEXT(" \"HMD\" : \"COSMOS\",\n")
				TEXT(" \n")
				TEXT(" \"RenderWidth\" : 1440,\n")
				TEXT(" \"RenderHeight\" : 1600,\n")
				TEXT(" \"RenderSizeScale\" : 1.0,\n")
				TEXT(" \"RenderOverfillScale\" : 1.1,\n")
				TEXT(" \n")
				TEXT(" \"UseAutoPrecdictTime\" : true,\n")
				TEXT(" \"CtlPredictRate\" : 6,\n")
				TEXT(" \"HmdPredictRatio\" : 0.615,\n")
				TEXT(" \"CtlPredictRatio\" : 0.615,\n")
				TEXT(" \"HmdPredict\" : 41,\n")
				TEXT(" \"ControllerPredict\" : 40,\n")
				TEXT(" \"MaxHmdPredictTimeInMs\" : 35,\n")
				TEXT(" \"MaxCtlPredictTimeInMs\" : 20,\n")
				TEXT(" \n")
				TEXT(" \"RoomHeight\" : 1.6\n")
				TEXT("}\n")
			);
			FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*cn), cn.Len());
			delete FileHandle;
		}
	}
}

void UWaveVRItemActions::CheckConfigAndDelete(FString configFile) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*configFile)) {
		PlatformFile.DeleteFile(*configFile);
	}
}
