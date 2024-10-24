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

#include "WaveVRHandEnums.generated.h"

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandType : uint8
{
	Left,
	Right
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRGestureType : uint8
{
	// WVR_HandGestureType::WVR_HandGestureType_Invalid /**< The gesture is invalid. */
	Invalid		= 0,
	// WVR_HandGestureType::WVR_HandGestureType_Unknown /**< Unknown gesture type. */
	Unknown		= 1,
	// WVR_HandGestureType::WVR_HandGestureType_Fist /**< Represent fist gesture. */
	Fist		= 2,
	// WVR_HandGestureType::WVR_HandGestureType_Five /**< Represent five gesture. */
	Five		= 3,
	// WVR_HandGestureType::WVR_HandGestureType_OK /**< Represent ok gesture. */
	OK			= 4,
	// WVR_HandGestureType::WVR_HandGestureType_ThumbUp /**< Represent thumb up gesture. */
	Like		= 5,
	// WVR_HandGestureType::WVR_HandGestureType_IndexUp /**< Represent index up gesture. */
	Point		= 6,
	// WVR_HandGestureType::WVR_HandGestureType_Palm_Pinch
	Palm_Pinch	= 7,
	// WVR_HandGestureType::WVR_HandGestureType_Yeah /**< Represent yeah gesture. */
	Yeah		= 8,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRTrackerType : uint8
{
	Invalid = 0,
	// WVR_HandTrackerType::WVR_HandTrackerType_Natural
	Natural = 1,
	// WVR_HandTrackerType::WVR_HandTrackerType_Electronic
	Electronic = 2,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandMotion : uint8
{
	// WVR_HandPoseType::WVR_HandPoseType_Invalid
	Invalid = 0,
	// WVR_HandPoseType::WVR_HandPoseType_Pinch
	Pinch = 1,
	// WVR_HandPoseType::WVR_HandPoseType_Hold
	Hold = 2,
};

static const unsigned int EWaveVRHandJointCount = 26;
UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandJoint : uint8
{
	// WVR_HandJoint::WVR_HandJoint_Palm
	Palm = 0,
	// WVR_HandJoint::WVR_HandJoint_Wrist
	Wrist = 1,
	// WVR_HandJoint::WVR_HandJoint_Thumb_Joint0
	Thumb_Joint0	= 2,
	// WVR_HandJoint::WVR_HandJoint_Thumb_Joint1
	Thumb_Joint1	= 3,
	// WVR_HandJoint::WVR_HandJoint_Thumb_Joint2
	Thumb_Joint2	= 4,
	// WVR_HandJoint::WVR_HandJoint_Thumb_Tip
	Thumb_Tip		= 5,
	// WVR_HandJoint::WVR_HandJoint_Index_Joint0
	Index_Joint0	= 6,
	// WVR_HandJoint::WVR_HandJoint_Index_Joint1
	Index_Joint1	= 7,
	// WVR_HandJoint::WVR_HandJoint_Index_Joint2
	Index_Joint2	= 8,
	// WVR_HandJoint::WVR_HandJoint_Index_Joint3
	Index_Joint3	= 9,
	// WVR_HandJoint::WVR_HandJoint_Index_Tip
	Index_Tip		= 10,
	// WVR_HandJoint::WVR_HandJoint_Middle_Joint0
	Middle_Joint0	= 11,
	// WVR_HandJoint::WVR_HandJoint_Middle_Joint1
	Middle_Joint1	= 12,
	// WVR_HandJoint::WVR_HandJoint_Middle_Joint2
	Middle_Joint2	= 13,
	// WVR_HandJoint::WVR_HandJoint_Middle_Joint3
	Middle_Joint3	= 14,
	// WVR_HandJoint::WVR_HandJoint_Middle_Tip
	Middle_Tip		= 15,
	// WVR_HandJoint::WVR_HandJoint_Ring_Joint0
	Ring_Joint0		= 16,
	// WVR_HandJoint::WVR_HandJoint_Ring_Joint1
	Ring_Joint1		= 17,
	// WVR_HandJoint::WVR_HandJoint_Ring_Joint2
	Ring_Joint2		= 18,
	// WVR_HandJoint::WVR_HandJoint_Ring_Joint3,
	Ring_Joint3		= 19,
	// WVR_HandJoint::WVR_HandJoint_Ring_Tip
	Ring_Tip		= 20,
	// WVR_HandJoint::WVR_HandJoint_Pinky_Joint0
	Pinky_Joint0	= 21,
	// WVR_HandJoint::WVR_HandJoint_Pinky_Joint1
	Pinky_Joint1	= 22,
	// WVR_HandJoint::WVR_HandJoint_Pinky_Joint2
	Pinky_Joint2	= 23,
	// WVR_HandJoint::WVR_HandJoint_Pinky_Joint3,
	Pinky_Joint3	= 24,
	// WVR_HandJoint::WVR_HandJoint_Pinky_Tip
	Pinky_Tip		= 25,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandHoldObjectType : uint8
{
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_None
	None = 0,
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_Gun
	Gun = 1,
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_OCSpray
	OCSpray = 2,
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_LongGun
	LongGun = 3,
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_Baton
	Baton = 4,
	// WVR_HandHoldObjectType::WVR_HandHoldObjectType_FlashLight
	FlashLight = 5,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandHoldRoleType : uint8
{
	// WVR_HandHoldRoleType::WVR_HandHoldRoleType_None
	None = 0,
	// WVR_HandHoldRoleType::WVR_HandHoldRoleType_MainHold
	MainHold = 1,
	// WVR_HandHoldRoleType::WVR_HandHoldRoleType_SideHold
	SideHold = 2,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRFingerType : uint8
{
	None = 0,
	// WVR_FingerType::WVR_FingerType_Thumb
	Thumb = 1,
	// WVR_FingerType::WVR_FingerType_Index
	Index = 2,
	// WVR_FingerType::WVR_FingerType_Middle
	Middle = 3,
	// WVR_FingerType::WVR_FingerType_Ring
	Ring = 4,
	// WVR_FingerType::WVR_FingerType_Pinky
	Pinky = 5,
};

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandGestureStatus : uint8
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

UENUM(BlueprintType, Category = "WaveVR|Hand")
enum class EWaveVRHandTrackingStatus : uint8
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
