// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRCommands.h"

#define LOCTEXT_NAMESPACE "FWaveVRModule"

void FWaveVRCommands::RegisterCommands()
{
	//UI_COMMAND(PluginAction, "Example", "Tooltips", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StartDPServer, "Start Streaming Server", "Supports Wi-Fi and USB connection types and automatically switches connection types.\nWarning: Remember to Stop Streaming Server if you want to start VIVE Business Streaming or VIVE Streaming Hub for PC Streaming.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StopDPServer, "Stop Streaming Server", "Supports Wi-Fi and USB connection types and automatically switches connection types.\nWarning: Remember to Stop Streaming Server if you want to start VIVE Business Streaming or VIVE Streaming Hub for PC Streaming.", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(InstallDeviceAPK, "Install Device APK", "Install Direct Preview APK to HMD and configure (Please make sure the HMD device can be recognized through adb command in cmd prompt)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StartDeviceAPK, "Start VIVE Business", "Start Direct Preview APK (Please make sure the HMD device can be recognized through adb command in cmd prompt)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StopDeviceAPK, "Stop VIVE Business", "Stop Direct Preview APK (Please make sure the HMD device can be recognized through adb command in cmd prompt)", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
