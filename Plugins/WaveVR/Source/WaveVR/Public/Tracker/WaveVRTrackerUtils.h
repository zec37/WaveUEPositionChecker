// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "WaveVRTrackerUtils.generated.h"

#pragma region
const int kTrackerCount = 16; // depends on EWaveVRTrackerId

UENUM(BlueprintType, Category = "WaveVR|Tracker")
enum class EWaveVRTrackerId : uint8
{
	Tracker0 = 0, //WVR_TrackerId.WVR_TrackerId_0
	Tracker1 = 1, //WVR_TrackerId.WVR_TrackerId_1
	Tracker2 = 2, //WVR_TrackerId.WVR_TrackerId_2,
	Tracker3 = 3, //WVR_TrackerId.WVR_TrackerId_3,
	Tracker4 = 4, //WVR_TrackerId.WVR_TrackerId_4,
	Tracker5 = 5, //WVR_TrackerId.WVR_TrackerId_5,
	Tracker6 = 6, //WVR_TrackerId.WVR_TrackerId_6,
	Tracker7 = 7, //WVR_TrackerId.WVR_TrackerId_7,
	Tracker8 = 8, //WVR_TrackerId.WVR_TrackerId_8,
	Tracker9 = 9, //WVR_TrackerId.WVR_TrackerId_9,
	Tracker10 = 10, //WVR_TrackerId.WVR_TrackerId_10,
	Tracker11 = 11, //WVR_TrackerId.WVR_TrackerId_11,
	Tracker12 = 12, //WVR_TrackerId.WVR_TrackerId_12,
	Tracker13 = 13, //WVR_TrackerId.WVR_TrackerId_13,
	Tracker14 = 14, //WVR_TrackerId.WVR_TrackerId_14,
	Tracker15 = 15, //WVR_TrackerId.WVR_TrackerId_15,
};
const EWaveVRTrackerId k_TrackerIds[kTrackerCount] =
{
	EWaveVRTrackerId::Tracker0,
	EWaveVRTrackerId::Tracker1,
	EWaveVRTrackerId::Tracker2,
	EWaveVRTrackerId::Tracker3,
	EWaveVRTrackerId::Tracker4,
	EWaveVRTrackerId::Tracker5,
	EWaveVRTrackerId::Tracker6,
	EWaveVRTrackerId::Tracker7,
	EWaveVRTrackerId::Tracker8,
	EWaveVRTrackerId::Tracker9,
	EWaveVRTrackerId::Tracker10,
	EWaveVRTrackerId::Tracker11,
	EWaveVRTrackerId::Tracker12,
	EWaveVRTrackerId::Tracker13,
	EWaveVRTrackerId::Tracker14,
	EWaveVRTrackerId::Tracker15,
};
#pragma endregion Tracker ID

UENUM(BlueprintType, Category = "WaveVR|Tracker")
enum class EWaveVRTrackerRole : uint8
{
	Undefined   = 0,  // WVR_TrackerRole.WVR_TrackerRole_Undefined,
	Standalone  = 1,  // WVR_TrackerRole.WVR_TrackerRole_Standalone,
	Pair1_Right = 2,  // WVR_TrackerRole.WVR_TrackerRole_Pair1_Right,
	Pair1_Left  = 3,  // WVR_TrackerRole.WVR_TrackerRole_Pair1_Left,

	Shoulder_Right = 32, // WVR_TrackerRole_Shoulder_Right
	Upper_Arm_Right = 33, // WVR_TrackerRole_Upper_Arm_Right
	Elbow_Right = 34, // WVR_TrackerRole_Elbow_Right
	Forearm_Right = 35, // WVR_TrackerRole_Forearm_Right
	Wrist_Right = 36, // WVR_TrackerRole_Wrist_Right
	Hand_Right = 37, // WVR_TrackerRole_Hand_Right
	Thigh_Right = 38, // WVR_TrackerRole_Thigh_Right
	Knee_Right = 39, // WVR_TrackerRole_Knee_Right
	Calf_Right = 40, // WVR_TrackerRole_Calf_Right
	Ankle_Right = 41, // WVR_TrackerRole_Ankle_Right
	Foot_Right = 42, // WVR_TrackerRole_Foot_Right

	Shoulder_Left = 47, // WVR_TrackerRole_Shoulder_Left
	Upper_Arm_Left = 48, // WVR_TrackerRole_Upper_Arm_Left
	Elbow_Left = 49, // WVR_TrackerRole_Elbow_Left
	Forearm_Left = 50, // WVR_TrackerRole_Forearm_Left
	Wrist_Left = 51, // WVR_TrackerRole_Wrist_Left
	Hand_Left = 52, // WVR_TrackerRole_Hand_Left
	Thigh_Left = 53, // WVR_TrackerRole_Thigh_Left
	Knee_Left = 54, // WVR_TrackerRole_Knee_Left
	Calf_Left = 55, // WVR_TrackerRole_Calf_Left
	Ankle_Left = 56, // WVR_TrackerRole_Ankle_Left
	Foot_Left = 57, // WVR_TrackerRole_Foot_Left

	Chest = 62, // WVR_TrackerRole_Chest
	Waist = 63, // WVR_TrackerRole_Waist

	Camera = 71, // WVR_TrackerRole_Camera
	Keyboard = 72, // WVR_TrackerRole_Keyboard
};

#pragma region Tracker Button
const int kTrackerButtonCount = 9;

UENUM(BlueprintType, Category = "WaveVR|Tracker")
enum class EWaveVRTrackerButton : uint8
{
	System		= 0,//WVR_InputId.WVR_InputId_0
	Menu		= 1,//WVR_InputId.WVR_InputId_Alias1_Menu
	Grip		= 2,//WVR_InputId.WVR_InputId_Alias1_Grip
	A			= 10,//WVR_InputId.WVR_InputId_Alias1_A
	B			= 11,//WVR_InputId.WVR_InputId_Alias1_B
	X			= 12,//WVR_InputId.WVR_InputId_Alias1_X
	Y			= 13,//WVR_InputId.WVR_InputId_Alias1_Y
	Touchpad	= 16,//WVR_InputId.WVR_InputId_Alias1_Touchpad
	Trigger		= 17,//WVR_InputId.WVR_InputId_Alias1_Trigger
};

const EWaveVRTrackerButton k_TrackerButtons[kTrackerButtonCount] =
{
	EWaveVRTrackerButton::System,
	EWaveVRTrackerButton::Menu,
	EWaveVRTrackerButton::Grip,
	EWaveVRTrackerButton::A,
	EWaveVRTrackerButton::B,
	EWaveVRTrackerButton::X,
	EWaveVRTrackerButton::Y,
	EWaveVRTrackerButton::Touchpad,
	EWaveVRTrackerButton::Trigger,
};
#pragma endregion

UENUM(BlueprintType, Category = "WaveVR|Tracker")
enum class EWaveVRTrackerStatus : uint8
{
	// Initial, can call Start API in this state.
	NOT_START,
	START_FAILURE,

	// Processing, should NOT call API in this state.
	STARTING,
	STOPING,

	// Running, can call Stop API in this state.
	AVAILABLE,

	// Do nothing.
	UNSUPPORT
};
