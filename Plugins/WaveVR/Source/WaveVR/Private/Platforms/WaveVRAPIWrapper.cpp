// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRAPIWrapper.h"

#include "WaveVRUtils.h"

#include "Platforms/WaveVRLogWrapper.h"

DEFINE_LOG_CATEGORY_STATIC(WVRAPIWrapper, Log, All);
using namespace wvr::utils;

#pragma region
FVector BONE_L_POS_OFFSET = FVector(15, 0, 0);
FVector BONE_L_POS_FUSION_OFFSET = FVector(5, 0, 0);
// Left wrist.
FVector BONE_HAND_WRIST_L_POS		= FVector(20.0f, -9, 0);
FRotator BONE_HAND_WRIST_L_ROT		= FRotator(165, 0, -7);
// Left thumb.
FVector BONE_THUMB_JOINT2_L_POS		= FVector(20.0f, -5, 2);
FVector BONE_THUMB_JOINT3_L_POS		= FVector(20.0f, -4, 3);
FVector BONE_THUMB_TIP_L_POS		= FVector(20.0f, -3, 4);
// Left index.
FVector BONE_INDEX_JOINT1_L_POS		= FVector(20.0f, -6, 4);
FVector BONE_INDEX_JOINT2_L_POS		= FVector(20.0f, -5.6f, 5);
FVector BONE_INDEX_JOINT3_L_POS		= FVector(20.0f, -5.2f, 6);
FVector BONE_INDEX_TIP_L_POS		= FVector(20.0f, -4.8f, 7);
// Left middle.
FVector BONE_MIDDLE_JOINT1_L_POS	= FVector(20.0f, -7.5f, 4.5f);
FVector BONE_MIDDLE_JOINT2_L_POS	= FVector(20.0f, -7.4f, 5.5f);
FVector BONE_MIDDLE_JOINT3_L_POS	= FVector(20.0f, -7.3f, 6.5f);
FVector BONE_MIDDLE_TIP_L_POS		= FVector(20.0f, -7.2f, 7.5f);
// Left ring.
FVector BONE_RING_JOINT1_L_POS		= FVector(20.0f, -8.7f, 4);
FVector BONE_RING_JOINT2_L_POS		= FVector(20.0f, -8.9f, 5);
FVector BONE_RING_JOINT3_L_POS		= FVector(20.0f, -9.1f, 6);
FVector BONE_RING_TIP_L_POS			= FVector(20.0f, -9.3f, 7);
// Left pinky.
FVector BONE_PINKY_JOINT1_L_POS		= FVector(20.0f, -9.9f, 3);
FVector BONE_PINKY_JOINT2_L_POS		= FVector(20.0f, -10.3f, 4);
FVector BONE_PINKY_JOINT3_L_POS		= FVector(20.0f, -10.6f, 5);
FVector BONE_PINKY_TIP_L_POS		= FVector(20.0f, -10.9f, 6);

FVector BONE_R_POS_OFFSET = FVector(15, 0, 0);
FVector BONE_R_POS_FUSION_OFFSET = FVector(5, 0, 0);
// Right wrist.
FVector BONE_HAND_WRIST_R_POS		= FVector(20.0f, 9, 0);
FRotator BONE_HAND_WRIST_R_ROT		= FRotator(165, 0, 7);
// Right thumb.
FVector BONE_THUMB_JOINT2_R_POS		= FVector(20.0f, 5, 2);
FVector BONE_THUMB_JOINT3_R_POS		= FVector(20.0f, 4, 3);
FVector BONE_THUMB_TIP_R_POS	= FVector(20.0f, 3, 4);
// Right index.
FVector BONE_INDEX_JOINT1_R_POS		= FVector(20.0f, 6, 4);
FVector BONE_INDEX_JOINT2_R_POS		= FVector(20.0f, 5.6f, 5);
FVector BONE_INDEX_JOINT3_R_POS		= FVector(20.0f, 5.2f, 6);
FVector BONE_INDEX_TIP_R_POS		= FVector(20.0f, 4.8f, 7);
// Right middle.
FVector BONE_MIDDLE_JOINT1_R_POS	= FVector(20.0f, 7.5f, 4.5f);
FVector BONE_MIDDLE_JOINT2_R_POS	= FVector(20.0f, 7.4f, 5.5f);
FVector BONE_MIDDLE_JOINT3_R_POS	= FVector(20.0f, 7.3f, 6.5f);
FVector BONE_MIDDLE_TIP_R_POS		= FVector(20.0f, 7.2f, 7.5f);
// Right ring.
FVector BONE_RING_JOINT1_R_POS		= FVector(20.0f, 8.7f, 4);
FVector BONE_RING_JOINT2_R_POS		= FVector(20.0f, 8.9f, 5);
FVector BONE_RING_JOINT3_R_POS		= FVector(20.0f, 9.1f, 6);
FVector BONE_RING_TIP_R_POS			= FVector(20.0f, 9.3f, 7);
// Right pinky.
FVector BONE_PINKY_JOINT1_R_POS		= FVector(20.0f, 9.9f, 3);
FVector BONE_PINKY_JOINT2_R_POS		= FVector(20.0f, 10.3f, 4);
FVector BONE_PINKY_JOINT3_R_POS		= FVector(20.0f, 10.6f, 5);
FVector BONE_PINKY_TIP_R_POS		= FVector(20.0f, 10.9f, 6);

FVector PINCH_DIRECTION_R = FVector(1, -.4f, .5f), PINCH_DIRECTION_L = FVector(1, .4f, .5f);
#pragma endregion Hand Tracking data

#pragma region
FVector eyeGazeOrigin = FVector::ZeroVector;
FVector eyeGazeDirection = FVector(1, -0.5, 0);
int gazeDirectionArrayIndex = 0;
FVector gazeDirectionArray[] = {
	FVector(100, -50, 0),
	FVector(100, 0, 0),
	FVector(100, 50, 0)
};
#pragma endregion Eye Tracking data

FWaveVRAPIWrapper * FWaveVRAPIWrapper::instance = nullptr;

FWaveVRAPIWrapper::FWaveVRAPIWrapper()
	: PlayerController(nullptr)
	, focusController(WVR_DeviceType::WVR_DeviceType_Controller_Right)
{
	s_ButtonPressed.Add(WVR_DeviceType::WVR_DeviceType_Controller_Left);
	s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left].Init(false, kButtonCount);
	s_ButtonPressed.Add(WVR_DeviceType::WVR_DeviceType_Controller_Right);
	s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right].Init(false, kButtonCount);

	s_TrackerPressed.Add(WVR_TrackerId::WVR_TrackerId_0);
	s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_0].Init(false, kButtonCount);
	s_TrackerPressed.Add(WVR_TrackerId::WVR_TrackerId_1);
	s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_1].Init(false, kButtonCount);

	// Static Gesture
	gestureData.left = WVR_HandGestureType::WVR_HandGestureType_IndexUp;
	gestureData.right = WVR_HandGestureType::WVR_HandGestureType_OK;
}

FWaveVRAPIWrapper::~FWaveVRAPIWrapper()
{
	if (FWaveVRAPIWrapper::instance == this)
		FWaveVRAPIWrapper::instance = nullptr;
}

void FWaveVRAPIWrapper::SetInstance(FWaveVRAPIWrapper * inst)
{
	FWaveVRAPIWrapper::instance = inst;
}

const uint32_t kGestureCount = 9;
const WVR_HandGestureType s_Gestures[kGestureCount] =
{
	WVR_HandGestureType::WVR_HandGestureType_Invalid,
	WVR_HandGestureType::WVR_HandGestureType_Unknown,
	WVR_HandGestureType::WVR_HandGestureType_Fist,
	WVR_HandGestureType::WVR_HandGestureType_Five,
	WVR_HandGestureType::WVR_HandGestureType_OK,
	WVR_HandGestureType::WVR_HandGestureType_ThumbUp,
	WVR_HandGestureType::WVR_HandGestureType_IndexUp,
	WVR_HandGestureType::WVR_HandGestureType_Palm_Pinch,
	WVR_HandGestureType::WVR_HandGestureType_Yeah,
};
uint32_t gestureIndex = 0;

int32 tickCount = 0, tickAdder = 1;
FRotator leftYawRotator = FRotator(0, 0.1f, 0), rightYawRotator = FRotator(0, -0.1f, 0);
FQuat leftYawOrientation = FQuat::Identity, rightYawOrientation = FQuat::Identity;
float pinchStrengthL = 0.1f, pinchStrengthR = 0.3f;
int32 gestureCount = 0, eyeDataCount = 0, pinchCount = 0;
float graspStrengthL = 0.1f, graspStrengthR = 0.3f;
bool isGraspingL = false, isGraspingR = false;
void FWaveVRAPIWrapper::Tick()
{
	tickCount += tickAdder;
	if (tickCount == 100 || tickCount == -100)
	{
		tickAdder *= -1;
	}
	leftYawRotator.Yaw += 0.1f * tickAdder;
	rightYawRotator.Yaw += -0.1f * tickAdder;
	leftYawOrientation = leftYawRotator.Quaternion();
	rightYawOrientation = rightYawRotator.Quaternion();

	// Static Gesture
	gestureCount++;
	gestureCount %= 300;
	if (gestureCount == 0)
	{
		if (gestureData.left == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
			gestureData.left = WVR_HandGestureType::WVR_HandGestureType_Five;
		else
			gestureData.left = WVR_HandGestureType::WVR_HandGestureType_IndexUp;

		gestureIndex++;
		gestureIndex %= kGestureCount;
		gestureData.right = s_Gestures[gestureIndex];
	}

	// Pinch Strength
	pinchCount++;
	pinchCount %= 200;
	if (pinchCount == 0)
	{
		pinchStrengthL += 0.1f;
		if (pinchStrengthL > 1) { pinchStrengthL -= 1; }
		pinchStrengthR += 0.1f;
		if (pinchStrengthR > 1) { pinchStrengthR -= 1; }

		graspStrengthL += 0.1f;
		if (graspStrengthL > 1) { graspStrengthL -= 1; }
		isGraspingL = graspStrengthL > 0.5f;
		graspStrengthR += 0.1f;
		if (graspStrengthR > 1) { graspStrengthR -= 1; }
		isGraspingR = graspStrengthR > 0.7f;
	}

	/**
	 * Eye Tracking Data
	 **/
	eyeDataCount++;
	eyeDataCount %= 300;
	if (eyeDataCount == 0)
	{
		gazeDirectionArrayIndex++;
		gazeDirectionArrayIndex %= UE_ARRAY_COUNT(gazeDirectionArray);
		eyeData.combined.eyeTrackingValidBitMask = (uint64_t)
			(
				WVR_EyeTrackingStatus::WVR_GazeOriginValid |
				WVR_EyeTrackingStatus::WVR_GazeDirectionNormalizedValid
				);
		eyeData.combined.gazeDirectionNormalized = OpenglCoordinate::GetVector3(gazeDirectionArray[gazeDirectionArrayIndex]);
	}
}

static const uint32_t m_JointCount = 20;
WVR_Result FWaveVRAPIWrapper::GetHandJointCount(WVR_HandTrackerType type, uint32_t* jointCount)
{
	*jointCount = m_JointCount;
	return WVR_Result::WVR_Success;
}

const uint64_t kModelTypeBitMask = (uint64_t)(WVR_HandModelType::WVR_HandModelType_WithoutController/* | WVR_HandModelType::WVR_HandModelType_WithController*/);
const uint64_t kJointValidFlag = (uint64_t)(WVR_HandJointValidFlag::WVR_HandJointValidFlag_PositionValid/* | WVR_HandJointValidFlag::WVR_HandJointValidFlag_RotationValid*/);
WVR_HandJoint s_HandJoints[m_JointCount];
uint64_t s_HandJointsFlag[m_JointCount];
WVR_Result FWaveVRAPIWrapper::GetHandTrackerInfo(WVR_HandTrackerType type, WVR_HandTrackerInfo_t* info)
{
	info->jointCount = m_JointCount;
	info->handModelTypeBitMask = kModelTypeBitMask;

	s_HandJoints[0] = WVR_HandJoint::WVR_HandJoint_Wrist;

	s_HandJoints[1] = WVR_HandJoint::WVR_HandJoint_Thumb_Joint1;
	s_HandJoints[2] = WVR_HandJoint::WVR_HandJoint_Thumb_Joint2;
	s_HandJoints[3] = WVR_HandJoint::WVR_HandJoint_Thumb_Tip;

	s_HandJoints[4] = WVR_HandJoint::WVR_HandJoint_Index_Joint1;
	s_HandJoints[5] = WVR_HandJoint::WVR_HandJoint_Index_Joint2;
	s_HandJoints[6] = WVR_HandJoint::WVR_HandJoint_Index_Joint3;
	s_HandJoints[7] = WVR_HandJoint::WVR_HandJoint_Index_Tip;

	s_HandJoints[8] = WVR_HandJoint::WVR_HandJoint_Middle_Joint1;
	s_HandJoints[9] = WVR_HandJoint::WVR_HandJoint_Middle_Joint2;
	s_HandJoints[10] = WVR_HandJoint::WVR_HandJoint_Middle_Joint3;
	s_HandJoints[11] = WVR_HandJoint::WVR_HandJoint_Middle_Tip;

	s_HandJoints[12] = WVR_HandJoint::WVR_HandJoint_Ring_Joint1;
	s_HandJoints[13] = WVR_HandJoint::WVR_HandJoint_Ring_Joint2;
	s_HandJoints[14] = WVR_HandJoint::WVR_HandJoint_Ring_Joint3;
	s_HandJoints[15] = WVR_HandJoint::WVR_HandJoint_Ring_Tip;

	s_HandJoints[16] = WVR_HandJoint::WVR_HandJoint_Pinky_Joint1;
	s_HandJoints[17] = WVR_HandJoint::WVR_HandJoint_Pinky_Joint2;
	s_HandJoints[18] = WVR_HandJoint::WVR_HandJoint_Pinky_Joint3;
	s_HandJoints[19] = WVR_HandJoint::WVR_HandJoint_Pinky_Tip;

	info->jointMappingArray = s_HandJoints;

	for (uint32_t i = 0; i < m_JointCount; i++)
		s_HandJointsFlag[i] = kJointValidFlag;

	info->jointValidFlagArray = s_HandJointsFlag;

	info->pinchTHR = 0.6f;
	info->pinchOff = 0.4f;

	return WVR_Result::WVR_Success;
}

WVR_Pose_t m_JointsPoseLeft[m_JointCount], m_JointsPoseRight[m_JointCount];
WVR_Result FWaveVRAPIWrapper::GetHandTrackingData(WVR_HandTrackerType trackerType,
	WVR_HandModelType modelType,
	WVR_PoseOriginModel originModel,
	WVR_HandTrackingData* skeleton,
	WVR_HandPoseData_t* pose)
{
	skeleton->left.isValidPose = true;
	skeleton->left.confidence = 0.8f;
	skeleton->left.jointCount = m_JointCount;

	FVector BONE_L_OFFSET = fusionBracelet ? BONE_L_POS_FUSION_OFFSET : BONE_L_POS_OFFSET;
	// Left wrist.
	m_JointsPoseLeft[0].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_HAND_WRIST_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[0].rotation = OpenglCoordinate::GetQuaternion(BONE_HAND_WRIST_L_ROT);
	// Left thumb.
	m_JointsPoseLeft[1].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_JOINT2_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[2].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_JOINT3_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[3].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_THUMB_TIP_L_POS + BONE_L_OFFSET));
	// Left index.
	m_JointsPoseLeft[4].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT1_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[5].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT2_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[6].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_JOINT3_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[7].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_INDEX_TIP_L_POS + BONE_L_OFFSET));
	// Left middle.
	m_JointsPoseLeft[8].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT1_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[9].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT2_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[10].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_JOINT3_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[11].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_MIDDLE_TIP_L_POS + BONE_L_OFFSET));
	// Left ring.
	m_JointsPoseLeft[12].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT1_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[13].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT2_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[14].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_JOINT3_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[15].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_RING_TIP_L_POS + BONE_L_OFFSET));
	// Left pinky.
	m_JointsPoseLeft[16].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT1_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[17].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT2_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[18].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_JOINT3_L_POS + BONE_L_OFFSET));
	m_JointsPoseLeft[19].position = OpenglCoordinate::GetVector3(leftYawOrientation * (BONE_PINKY_TIP_L_POS + BONE_L_OFFSET));

	skeleton->left.joints = m_JointsPoseLeft;
	skeleton->left.scale.v[0] = 1;
	skeleton->left.scale.v[1] = 1;
	skeleton->left.scale.v[2] = 1;

	skeleton->left.wristLinearVelocity.v[0] = 0.1f;
	skeleton->left.wristLinearVelocity.v[1] = 0.2f;
	skeleton->left.wristLinearVelocity.v[2] = 0.3f;
	skeleton->left.wristAngularVelocity.v[0] = 0.1f;
	skeleton->left.wristAngularVelocity.v[1] = 0.2f;
	skeleton->left.wristAngularVelocity.v[2] = 0.3f;

	skeleton->left.grasp.strength = graspStrengthL;
	skeleton->left.grasp.isGrasp = isGraspingL;

	skeleton->right.isValidPose = true;
	skeleton->right.confidence = 0.8f;
	skeleton->right.jointCount = m_JointCount;


	FVector BONE_R_OFFSET = fusionBracelet ? BONE_R_POS_FUSION_OFFSET : BONE_R_POS_OFFSET;
	// Right wrist.
	m_JointsPoseRight[0].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_HAND_WRIST_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[0].rotation = OpenglCoordinate::GetQuaternion(BONE_HAND_WRIST_R_ROT);
	// Right thumb.
	m_JointsPoseRight[1].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_JOINT2_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[2].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_JOINT3_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[3].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_THUMB_TIP_R_POS + BONE_R_OFFSET));
	// Right index.
	m_JointsPoseRight[4].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT1_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[5].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT2_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[6].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_JOINT3_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[7].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_INDEX_TIP_R_POS + BONE_R_OFFSET));
	// Right middle.
	m_JointsPoseRight[8].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT1_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[9].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT2_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[10].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_JOINT3_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[11].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_MIDDLE_TIP_R_POS + BONE_R_OFFSET));
	// Right ring.
	m_JointsPoseRight[12].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT1_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[13].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT2_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[14].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_JOINT3_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[15].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_RING_TIP_R_POS + BONE_R_OFFSET));
	// Right pinky.
	m_JointsPoseRight[16].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT1_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[17].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT2_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[18].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_JOINT3_R_POS + BONE_R_OFFSET));
	m_JointsPoseRight[19].position = OpenglCoordinate::GetVector3(rightYawOrientation * (BONE_PINKY_TIP_R_POS + BONE_R_OFFSET));

	skeleton->right.joints = m_JointsPoseRight;
	skeleton->right.scale.v[0] = 0.5f;
	skeleton->right.scale.v[1] = 0.5f;
	skeleton->right.scale.v[2] = 0.5f;

	skeleton->right.wristLinearVelocity.v[0] = 0.11f;
	skeleton->right.wristLinearVelocity.v[1] = 0.21f;
	skeleton->right.wristLinearVelocity.v[2] = 0.31f;
	skeleton->right.wristAngularVelocity.v[0] = 0.11f;
	skeleton->right.wristAngularVelocity.v[1] = 0.21f;
	skeleton->right.wristAngularVelocity.v[2] = 0.31f;

	skeleton->right.grasp.strength = graspStrengthR;
	skeleton->right.grasp.isGrasp = isGraspingR;

	/// Pinch data
	pose->left.base.type = WVR_HandPoseType::WVR_HandPoseType_Pinch;
	pose->left.pinch.strength = pinchStrengthL;
	pose->left.pinch.origin = m_JointsPoseLeft[0].position;
	pose->left.pinch.direction = OpenglCoordinate::GetVector3(PINCH_DIRECTION_L);
	pose->left.pinch.isPinching = false;

	pose->right.base.type = WVR_HandPoseType::WVR_HandPoseType_Pinch;
	pose->right.pinch.base.type = WVR_HandPoseType::WVR_HandPoseType_Pinch;
	pose->right.pinch.finger = WVR_FingerType::WVR_FingerType_Index;
	pose->right.pinch.strength = pinchStrengthR;
	pose->right.pinch.origin = m_JointsPoseRight[0].position;
	pose->right.pinch.direction = OpenglCoordinate::GetVector3(PINCH_DIRECTION_R);
	pose->right.pinch.isPinching = true;

	/// Hold data
	/*
	pose->left.base.type = WVR_HandPoseType::WVR_HandPoseType_Hold;
	pose->left.hold.role = WVR_HandHoldRoleType::WVR_HandHoldRoleType_SideHold;
	pose->left.hold.object = WVR_HandHoldObjectType::WVR_HandHoldObjectType_Gun;

	pose->right.base.type = WVR_HandPoseType::WVR_HandPoseType_Hold;
	pose->right.hold.role = WVR_HandHoldRoleType::WVR_HandHoldRoleType_MainHold;
	pose->right.hold.object = WVR_HandHoldObjectType::WVR_HandHoldObjectType_Gun;
	*/

	return WVR_Result::WVR_Success;
}

bool FWaveVRAPIWrapper::AllowPollEventQueue()
{
	if (framePollEventQueue != GFrameCounter)
	{
		framePollEventQueue = GFrameCounter;
		return true;
	}
	return false;
}
void FWaveVRAPIWrapper::CheckPlayerController()
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.World()->IsPlayInEditor())
		{
			PlayerController = UGameplayStatics::GetPlayerController(Context.World(), 0);
			return;
		}
	}

	PlayerController = nullptr;
}
bool FWaveVRAPIWrapper::PollEventQueue(WVR_Event_t* event)
{
	if (!AllowPollEventQueue()) { return false; }

	CheckPlayerController();
	if (PlayerController == nullptr || event == nullptr) { return false; }
	PlayerController->bEnableClickEvents = true;

	// Left buttons
	if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][(uint8)WVR_InputId::WVR_InputId_Alias1_Touchpad])
			return false;

		s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][(uint8)WVR_InputId::WVR_InputId_Alias1_Touchpad] = true;

		event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonPressed;
		event->input.device.common.timestamp = GFrameCounter;
		event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Left;
		event->input.inputId = WVR_InputId::WVR_InputId_Alias1_Touchpad;
		return true;
	}
	if (PlayerController->IsInputKeyDown(EKeys::R))
	{
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][(uint8)WVR_InputId::WVR_InputId_Alias1_Trigger])
			return false;

		s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][(uint8)WVR_InputId::WVR_InputId_Alias1_Trigger] = true;

		event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonPressed;
		event->input.device.common.timestamp = GFrameCounter;
		event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Left;
		event->input.inputId = WVR_InputId::WVR_InputId_Alias1_Trigger;
		return true;
	}

	// Right buttons
	if (PlayerController->IsInputKeyDown(EKeys::RightMouseButton))
	{
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][(uint8)WVR_InputId::WVR_InputId_Alias1_Touchpad])
			return false;

		s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][(uint8)WVR_InputId::WVR_InputId_Alias1_Touchpad] = true;

		event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonPressed;
		event->input.device.common.timestamp = GFrameCounter;
		event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Right;
		event->input.inputId = WVR_InputId::WVR_InputId_Alias1_Touchpad;
		return true;
	}
	if (PlayerController->IsInputKeyDown(EKeys::T))
	{
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][(uint8)WVR_InputId::WVR_InputId_Alias1_Trigger])
			return false;

		s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][(uint8)WVR_InputId::WVR_InputId_Alias1_Trigger] = true;

		event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonPressed;
		event->input.device.common.timestamp = GFrameCounter;
		event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Right;
		event->input.inputId = WVR_InputId::WVR_InputId_Alias1_Trigger;
		return true;
	}

	// Tracker buttons
	if (PlayerController->IsInputKeyDown(EKeys::P))
	{
		if (s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_0][(uint8)WVR_InputId::WVR_InputId_Alias1_A])
			return false;

		s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_0][(uint8)WVR_InputId::WVR_InputId_Alias1_A] = true;

		event->trackerInput.tracker.common.type = WVR_EventType::WVR_EventType_TrackerButtonPressed;
		event->trackerInput.tracker.common.timestamp = GFrameCounter;
		event->trackerInput.tracker.trackerId = WVR_TrackerId::WVR_TrackerId_0;
		event->trackerInput.inputId = WVR_InputId::WVR_InputId_Alias1_A;
		return true;
	}
	if (PlayerController->IsInputKeyDown(EKeys::Q))
	{
		if (s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_1][(uint8)WVR_InputId::WVR_InputId_Alias1_X])
			return false;

		s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_1][(uint8)WVR_InputId::WVR_InputId_Alias1_X] = true;

		event->trackerInput.tracker.common.type = WVR_EventType::WVR_EventType_TrackerButtonPressed;
		event->trackerInput.tracker.common.timestamp = GFrameCounter;
		event->trackerInput.tracker.trackerId = WVR_TrackerId::WVR_TrackerId_1;
		event->trackerInput.inputId = WVR_InputId::WVR_InputId_Alias1_X;
		return true;
	}

	// No button pressed
	for (int i = 0; i < kButtonCount; i++)
	{
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][i])
		{
			s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Left][i] = false;

			event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonUnpressed;
			event->input.device.common.timestamp = GFrameCounter;
			event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Left;
			event->input.inputId = (WVR_InputId)i;
			return true;
		}
		if (s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][i])
		{
			s_ButtonPressed[WVR_DeviceType::WVR_DeviceType_Controller_Right][i] = false;

			event->input.device.common.type = WVR_EventType::WVR_EventType_ButtonUnpressed;
			event->input.device.common.timestamp = GFrameCounter;
			event->input.device.deviceType = WVR_DeviceType::WVR_DeviceType_Controller_Right;
			event->input.inputId = (WVR_InputId)i;
			return true;
		}
		if (s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_0][i])
		{
			s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_0][i] = false;

			event->trackerInput.tracker.common.type = WVR_EventType::WVR_EventType_TrackerButtonUnpressed;
			event->trackerInput.tracker.common.timestamp = GFrameCounter;
			event->trackerInput.tracker.trackerId = WVR_TrackerId::WVR_TrackerId_0;
			event->trackerInput.inputId = WVR_InputId::WVR_InputId_Alias1_A;
			return true;
		}
		if (s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_1][i])
		{
			s_TrackerPressed[WVR_TrackerId::WVR_TrackerId_1][i] = false;

			event->trackerInput.tracker.common.type = WVR_EventType::WVR_EventType_TrackerButtonUnpressed;
			event->trackerInput.tracker.common.timestamp = GFrameCounter;
			event->trackerInput.tracker.trackerId = WVR_TrackerId::WVR_TrackerId_1;
			event->trackerInput.inputId = WVR_InputId::WVR_InputId_Alias1_X;
			return true;
		}
	}

	return false;
}


/* SceneMesh dummy data */
WVR_Result FWaveVRAPIWrapper::StartScenePerception(WVR_ScenePerceptionTarget target) {
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::StopScenePerception(WVR_ScenePerceptionTarget target) { return WVR_Success; }

WVR_Result FWaveVRAPIWrapper::GetScenePerceptionState(WVR_ScenePerceptionTarget target, WVR_ScenePerceptionState* state) {
	if (target == WVR_ScenePerceptionTarget_2dPlane && state != nullptr) {
		*state = WVR_ScenePerceptionState_Completed;
		return WVR_Success;
	}
	std::srand(time(NULL));
	if (state != nullptr) {
		int sps = std::rand() % 4;
		*state = (WVR_ScenePerceptionState)sps;
	}
	return WVR_Success;
}

namespace scene
{
	static void GenerateUUID(uint8* data, int index, bool zero) {
		if (zero) {
			for (int i = 0; i < WVR_UUID_SIZE; i++) {
				data[i] = 0;
			}
			return;
		}

		for (int i = 0; i < WVR_UUID_SIZE; i++) {
			data[i] = (uint8)((std::rand() / ((RAND_MAX + 1u) / 0x100) * i + index) & 0xFF);
		}
	}

	static const int QuadIndexBuffer[] = {
		0, 1, 2,
		0, 2, 3
	};

	static const float QuadVertexBuffer[] = {
		-1, -1, 0,
		 1, -1, 0,
		 1,  1, 0,
		-1,  1, 0
	};

#ifndef MAKE_SCENE_PLANE
	// Note: The w and h mean the reall width and height, not extent width and height
#define MAKE_SCENE_PLANE(_sp, type, label, idx, w, h, pos, rot) \
do { \
	auto& sp = (_sp); \
	sp.planeType = type; \
	sp.planeLabel = label; \
	sp.meshBufferId = 0x100 + idx; \
	sp.extent.width = w; \
	sp.extent.height = h; \
	sp.pose.position = pos; \
	sp.pose.rotation = rot; \
	sp.semanticName.name[0] = 'p'; \
	sp.semanticName.name[1] = 0; \
	scene::GenerateUUID(sp.uuid.data, idx, false); \
	scene::GenerateUUID(sp.parentUuid.data, 0, true); \
} while(false)
#endif

#ifndef MAKE_SCENE_OBJECT
// Note: The w and h mean the reall width and height, not extent width and height
#define MAKE_SCENE_OBJECT(_so, idx, w, h, d, pos, rot) \
do { \
	auto& so = (_so); \
	so.meshBufferId = 0x400 + idx; \
	so.extent.width = w; \
	so.extent.height = h; \
	so.extent.depth = d; \
	so.pose.position = pos; \
	so.pose.rotation = rot; \
	so.semanticName.name[0] = 'o'; \
	so.semanticName.name[1] = 0; \
	scene::GenerateUUID(so.uuid.data, idx, false); \
	scene::GenerateUUID(so.parentUuid.data, 0, true); \
} while(false)
#endif

	static inline WVR_Vector3f V3(float x, float y, float z) {
		WVR_Vector3f v;
		v.v[0] = x;
		v.v[1] = y;
		v.v[2] = z;
		return v;
	}

	static inline WVR_Quatf Q4(float x, float y, float z, float w) {
		WVR_Quatf q;
		q.x = x;
		q.y = y;
		q.z = z;
		q.w = w;
		return q;
	}

	static int PrepareDummyScenePlaneList(WVR_ScenePlane** listOut) {
		const int N = 9;
		static WVR_ScenePlane* WaveDummyScenePlaneList = nullptr;
		if (WaveDummyScenePlaneList != nullptr) {
			*listOut = WaveDummyScenePlaneList;
			return N;
		}
		WaveDummyScenePlaneList = new WVR_ScenePlane[N];
		auto& list = WaveDummyScenePlaneList;

		const WVR_ScenePlaneType TU = WVR_ScenePlaneType_HorizontalUpwardFacing;
		const WVR_ScenePlaneType TD = WVR_ScenePlaneType_HorizontalDownwardFacing;
		const WVR_ScenePlaneType TV = WVR_ScenePlaneType_Vertical;
		const WVR_ScenePlaneLabel LFL = WVR_ScenePlaneLabel_Floor;
		const WVR_ScenePlaneLabel LCL = WVR_ScenePlaneLabel_Ceiling;
		const WVR_ScenePlaneLabel LWL = WVR_ScenePlaneLabel_Wall;
		const WVR_ScenePlaneLabel LDK = WVR_ScenePlaneLabel_Desk;
		const WVR_ScenePlaneLabel LCC = WVR_ScenePlaneLabel_Couch;
		const WVR_ScenePlaneLabel LRR = WVR_ScenePlaneLabel_Door;
		const WVR_ScenePlaneLabel LWI = WVR_ScenePlaneLabel_Window;

		const float ND = 0.7071068f;
		// floor/celing w8*6d*4h
		const float W = 8;
		const float H = 4;
		const float D = 6;

		std::srand(time(NULL));

		// Desktop
		MAKE_SCENE_PLANE(list[0], TU, LDK, 0, 3, 1.5f, V3(0, 0.8f, -0.45f), Q4(-ND, 0, 0, ND));

		// floor/celing w8*6d*4h
		MAKE_SCENE_PLANE(list[1], TU, LFL, 1, W, D, V3(0, 0.01f, 0), Q4(-ND, 0, 0, ND));
		MAKE_SCENE_PLANE(list[2], TD, LCL, 2, W, D, V3(0, H, 0), Q4(ND, 0, 0, ND));

		// Wall postiion height(y) is 1/2 wall height.
		// front wall
		MAKE_SCENE_PLANE(list[3], TV, LWL, 3, W, H, V3(0, H / 2, -D / 2), Q4(0, 0, 0, 1));  // Z b
		// right wall
		MAKE_SCENE_PLANE(list[4], TV, LWL, 4, D, H, V3(W / 2, H / 2, 0), Q4(0, -ND, 0, ND));  // z r
		// left wall
		MAKE_SCENE_PLANE(list[5], TV, LWL, 5, D, H, V3(-W / 2, H / 2, 0), Q4(0, ND, 0, ND));  // z l
		// rear wall
		MAKE_SCENE_PLANE(list[6], TV, LWL, 6, W, H, V3(0, H / 2, D / 2), Q4(0, 1, 0, 0));  // z f

		// Moveable plane
		MAKE_SCENE_PLANE(list[7], TV, LWL, 7, 0.2f, 0.4f, V3(-0.5, 0.8f, -0.5f), Q4(0, 0, 0, 1));

		// Show/Hide plane (floor, face up)
		MAKE_SCENE_PLANE(list[8], TD, LFL, 8, 0.3f, 0.3f, V3(-0.5, 1.0f, -0.5f), Q4(-ND, 0, 0, ND));

		*listOut = WaveDummyScenePlaneList;
		return N;
	}

#ifdef MAKE_SCENE_PLANE
#undef MAKE_SCENE_PLANE
#endif

	static int PrepareDummySceneObjectList(WVR_SceneObject** listOut) {
		const int N = 2;
		static WVR_SceneObject* WaveDummySceneObjectList = nullptr;
		if (WaveDummySceneObjectList != nullptr) {
			*listOut = WaveDummySceneObjectList;
			return N;
		}
		WaveDummySceneObjectList = new WVR_SceneObject[N];
		auto& list = WaveDummySceneObjectList;

		const float ND = 0.7071068f;

		const float w = 0.5f;
		const float h = 1;
		const float d = 0.1f;

		std::srand(time(NULL));
		auto dirLeft = Q4(0, ND, 0, ND);
		auto dirFront = Q4(0, 1, 0, 0);

		MAKE_SCENE_OBJECT(list[0], 0, w, h, d, V3(1, 0.5f, -1), dirLeft);
		MAKE_SCENE_OBJECT(list[1], 0, w, h, d, V3(-1, 1, -1), dirFront);

		*listOut = WaveDummySceneObjectList;
		return N;
	}


#ifdef MAKE_SCENE_OBJECT
#undef MAKE_SCENE_OBJECT
#endif

	static WVR_Result GetScenePlaneMeshBuffer(uint64_t meshBufferId, WVR_SceneMeshBuffer* sceneMeshBuffer) {
		WVR_ScenePlane* list = nullptr;
		if (sceneMeshBuffer == nullptr)
			return WVR_Error_InvalidArgument;
		int N = PrepareDummyScenePlaneList(&list);
		int found = -1;
		for (int i = 0; i < N; i++) {
			if (list[i].meshBufferId == meshBufferId) {
				found = i;
				break;
			}
		}
		sceneMeshBuffer->indexCountOutput = 0;
		sceneMeshBuffer->vertexCountOutput = 0;
		if (found == -1) return WVR_Error_InvalidArgument;
		sceneMeshBuffer->indexCountOutput = 6;
		sceneMeshBuffer->vertexCountOutput = 4;
		if (!(sceneMeshBuffer->vertexBuffer != nullptr &&
			sceneMeshBuffer->indexBuffer != nullptr &&
			sceneMeshBuffer->indexCapacityInput >= 6 &&
			sceneMeshBuffer->vertexCapacityInput >= 4))
			return WVR_Success;

		// Make vertex and index by mesh description
		auto& plane = list[found];
		float* vb = (float*)sceneMeshBuffer->vertexBuffer;

		float* offset = plane.pose.position.v;
		float scaleX = plane.extent.width / 2;
		float scaleY = plane.extent.height / 2;

		for (int i = 0; i < 4; i++) {
			int x = i * 3, y = i * 3 + 1, z = i * 3 + 2;
			FQuat q = CoordinateUtil::GetQuaternion(plane.pose.rotation);
			WVR_Vector3f vw;
			vw.v[0] = scene::QuadVertexBuffer[x] * scaleX;
			vw.v[1] = scene::QuadVertexBuffer[y] * scaleY;
			vw.v[2] = scene::QuadVertexBuffer[z];

			//LOGD(WVRAPIWrapper, "v[%d] (%0.3f, %0.3f, %0.3f)", i, vw.v[0], vw.v[1], vw.v[2]);
			FVector vu = CoordinateUtil::GetVector3(vw, 100);

			WVR_Vector3f vwrot = OpenglCoordinate::GetVector3(q * vu);
			//LOGD(WVRAPIWrapper, "vwrot[%d] (%0.3f, %0.3f, %0.3f)", i, vwrot.v[0], vwrot.v[1], vwrot.v[2]);
			vb[x] = vwrot.v[0] + offset[0];
			vb[y] = vwrot.v[1] + offset[1];
			vb[z] = vwrot.v[2] + offset[2];
			//LOGD(WVRAPIWrapper, "vout[%d] (%0.3f, %0.3f, %0.3f)", i, vb[x], vb[y], vb[z]);
		}

		for (int i = 0; i < 6; i++)
			sceneMeshBuffer->indexBuffer[i] = scene::QuadIndexBuffer[i];

		return WVR_Success;
	}

	// D *-* C
	//   |/|
	// A *-* B  ABCD=0123
	//
	// z = f(a, b) = (1−a)(1−b)A + a(1−b)B + abC + (1−a)yD
	// where 0 <= x and y <= 1.
	static FVector BilinearInterpolation(float a, float b, FVector* corners) {
		auto& A = corners[0];
		auto& B = corners[1];
		auto& C = corners[2];
		auto& D = corners[3];
		return
			(1-a)*(1-b)*A + 
			a*(1-b)*B + 
			a*b*C + 
			(1-a)*b*D;
	}

	// Generate Vertices from four corners.
	// corner[0]=RearLeft, corner[1]=RearRight, corner[2]=FrontLeft, corner[3]=FrontRight
	// corners are in GL's tracking space where -Z is front.
	// D *-* C  <--- FrontRight
	//   |/|
	// A *-* B  ABCD index mapping to 0123
	// You should prepare output buffer size as a * b * 3.
	// The first output vertex will be corner A, and following vertices in a direction to corner B.
	static void VertexGenerator(int aCount, int bCount, FVector* corner, float heightScale, FVector* output) {
		LOGD(WVRAPIWrapper, "VertexGenerator SZ=%dx%d HS=%5.3f C0=%5.3f,%5.3f,%5.3f C1=%5.3f,%5.3f,%5.3f C2=%5.3f,%5.3f,%5.3f C3=%5.3f,%5.3f,%5.3f",
			aCount, bCount, heightScale,
			corner[0].X, corner[0].Y, corner[0].Z,
			corner[1].X, corner[1].Y, corner[1].Z,
			corner[2].X, corner[2].Y, corner[2].Z,
			corner[3].X, corner[3].Y, corner[3].Z
			);

		float stepA = 1.0f / (aCount - 1);
		float stepB = 1.0f / (bCount - 1);
		for (int j = 0; j < bCount; j++) {
			for (int i = 0; i < aCount; i++) {
				float heightDiff = (rand() / ((float)RAND_MAX * 0.5f)) - 1.0f;  // -1~1
				float genA = i * stepA;  // 0~1
				float genB = j * stepB;  // 0~1
				FVector bi = BilinearInterpolation(genA, genB, corner);
				bi.Y = bi.Y + heightScale * heightDiff;
				int idxO = i + j * bCount;
				//LOGD(WVRAPIWrapper, " %04X (%5.3f %5.3f %5.3f)", idxO, bi.X, bi.Y, bi.Z);
				output[idxO] = bi;
			}
		}
	}

	// You should prepare output buffer size as (a-1)*(b-1)*3*2.
	static void IndexGenerator(int aCount, int bCount, uint32_t* output) {
		LOGD(WVRAPIWrapper, "IndexGenerator");
		int idxIdx = 0;
		for (int j = 0; j < bCount - 1; j++) {
			for (int i = 0; i < aCount - 1; i++) {
				int a = i + j * bCount;
				int b = a + 1;
				int c = b + bCount;
				int d = c - 1;
				output[idxIdx++] = a;
				output[idxIdx++] = b;
				output[idxIdx++] = c;

				output[idxIdx++] = a;
				output[idxIdx++] = c;
				output[idxIdx++] = d;

				//LOGD(WVRAPIWrapper, " %04X %04X %04X - %04X %04X %04X", a, b, c, a, c, d);
			}
		}
	}

	static void FillSceneMeshBuffer(WVR_SceneMeshBuffer * buf, const FVector* vertices, int cV, const uint32_t* indices, int cI) {
		// XXX In UE5, FVector use double
		//LOGD(WVRAPIWrapper, " Sizeof FVector %u WVR_Vector3 %u", sizeof(FVector), sizeof(WVR_Vector3f_t));
		//check(sizeof(*vertices) == sizeof(*buf->vertexBuffer));
		check(sizeof(*indices) == sizeof(*buf->indexBuffer));

		buf->indexCapacityInput = 0;
		buf->indexCountOutput = cI;
		buf->indexBuffer = new uint32_t[cI];
		memcpy(buf->indexBuffer, indices, cI * sizeof(*buf->indexBuffer));

		buf->vertexCapacityInput = 0;
		buf->vertexCountOutput = cV;
		buf->vertexBuffer = new WVR_Vector3f_t[cV];
		if (sizeof(*vertices) == sizeof(*buf->vertexBuffer)) {
			memcpy(buf->vertexBuffer, vertices, cV * sizeof(*buf->vertexBuffer));
		} else {
			for (int i = 0; i < cV; i++) {
				buf->vertexBuffer[i].v[0] = vertices[i].X;
				buf->vertexBuffer[i].v[1] = vertices[i].Y;
				buf->vertexBuffer[i].v[2] = vertices[i].Z;
			}
		}
	}

	struct SceneMeshBuffer {
		WVR_SceneMeshBuffer b;
		int id;
	};

	static int PrepareDummySceneMeshBufferList(SceneMeshBuffer** listOut) {
		// Da and Db are be adjusted dynamically.
		const int Ca = 4;  // Count in a direction
		const int Cb = 4;  // Count in b direction
		const int Da = 64;  // Detail count in a direction
		const int Db = 64;  // Detail count in b direction
		WVR_SceneObject* listO = nullptr;
		const int NO = scene::PrepareDummySceneObjectList(&listO);  // Object count
		const int NM = (Ca - 1) * (Cb - 1);  // Terrain count
		const int N = NM + NO;  // total count
		static SceneMeshBuffer* SceneMeshBufferList = nullptr;
		if (SceneMeshBufferList != nullptr) {
			*listOut = SceneMeshBufferList;
			return N;
		}

		SceneMeshBufferList = new SceneMeshBuffer[N];
		auto& list = SceneMeshBufferList;

		std::srand(time(NULL));
		LOGD(WVRAPIWrapper, "PrepareDummySceneMeshBufferList() Start Generating:");
		// My base terrain shape and height 4x4:
		// 4-2-3-5 M N O P  <--- ( 8, 5,-7)
		// |/|/|/|
		// 1-0-0-1 I J K L
		// |/|/|/|    * (0,0,0)
		// 1-0-0-2 E F G H
		// |/|/|/|
		// 3-4-1-3 A B C D
		const FVector baseTerrainShape[Ca * Cb] = {
			FVector(-7, 3, 7),  // A
			FVector(-3, 4, 6),  // B
			FVector( 3, 1, 6),  // C
			FVector( 6, 3, 8),  // D

			FVector(-6, 1, 3),  // E
			FVector(-2, 0, 2),  // F
			FVector( 2, 0, 1),  // G
			FVector( 6, 2, 3),  // H

			FVector(-6, 1,-3),  // I
			FVector(-2, 0,-1),  // J
			FVector( 2, 0,-2),  // K
			FVector( 6, 1,-3),  // L

			FVector(-7, 4,-8),  // M
			FVector(-2, 2,-6),  // N
			FVector( 2, 3,-6),  // O
			FVector( 8, 5,-7),  // P
		};
		const int vC = Da * Db;
		const int iC = (Da - 1) * (Db - 1) * 3 * 2;
		FVector *vertexGen = new FVector[vC];
		uint32_t* indexGen = new uint32_t[iC];
		for (int j = 0; j < Cb - 1; j++) {
			for (int i = 0; i < Ca - 1; i++) {
				int idxSMBL = i + j * (Cb - 1);
				int idxBTSa = i + j * Cb;
				int idxBTSb = idxBTSa + 1;  //  (i + 1) + j * Cb;
				int idxBTSc = idxBTSb + Cb;  //  (i + 1) + (j + 1) * Cb;
				int idxBTSd = idxBTSc - 1;  // i + (j + 1) * Cb;
				FVector corners[4] = {
					baseTerrainShape[idxBTSa],
					baseTerrainShape[idxBTSb],
					baseTerrainShape[idxBTSc],
					baseTerrainShape[idxBTSd]
				};
				VertexGenerator(Da, Db, corners, 0.20f /*+-50cm dither*/, vertexGen);
				IndexGenerator(Da, Db, indexGen);
				auto b = (SceneMeshBufferList + idxSMBL);
				b->id = 0x800 + idxSMBL;
				LOGD(WVRAPIWrapper, "Mesh id=%X:", b->id);
				FillSceneMeshBuffer(&(b->b), vertexGen, vC, indexGen, iC);
			}
		}

		// Print debug info
		//for (int j = 0; j < Cb - 1; j++) {
		//	for (int i = 0; i < Ca - 1; i++) {
		//		int idxSMBL = i + j * (Cb - 1);
		//		auto* b = SceneMeshBufferList + idxSMBL;
		//		FVector* p = (FVector*)b->vertexBuffer;
		//		for (uint32 k = 0; k < b->vertexCountOutput; k++) {
		//			LOGD(WVRAPIWrapper, "[%04X] %5.3f, %5.3f, %5.3f", k, p[k].X, p[k].Y, p[k].Z);
		//		}
		//	}
		//}

		delete[] vertexGen;
		delete[] indexGen;

		vertexGen = nullptr;
		indexGen = nullptr;

		//     0------1
		//    /|     /|
		//   / |    / |   Y
		//  3------2  |   |
		//  |  4---|--5   *--X
		//  | /    | /   /
		//  |/     |/   Z
		//  7------6   
		// 
		// 1x1x1 Trapezoid with center (0, 0, 0)
		float ST = 0.5f;  // step
		const int ovC = 8;
		const FVector baseCubeShape[ovC] = {
			FVector(-ST,  ST, -ST),  // 0
			FVector(  0,  ST, -ST),  // 1
			FVector(  0,  ST,   0),  // 2
			FVector(-ST,  ST,   0),  // 3
			FVector(-ST, -ST, -ST),  // 4
			FVector( ST, -ST, -ST),  // 5
			FVector( ST, -ST,  ST),  // 6
			FVector(-ST, -ST,  ST),  // 7
		};

		const int oiC = 3 * 2 * 6;
		const uint32_t indicesCube[oiC] = {
			0, 2, 1, 0, 3, 2, // +Y
			4, 5, 6, 4, 6, 7, // -Y
			1, 2, 6, 1, 6, 5, // +X
			0, 7, 3, 0, 4, 7, // -X
			0, 1, 5, 0, 5, 4, // +Z
			2, 3, 7, 2, 7, 6, // -Z
		};
		
		vertexGen = new FVector[8];

		// Scene Object
		for (int idxSOBL = 0; idxSOBL < NO; idxSOBL++) {
			auto* b = SceneMeshBufferList + NM + idxSOBL;
			auto& object = listO[idxSOBL];

			FVector scale = FVector(object.extent.width, object.extent.height, object.extent.depth);
			for (int i = 0; i < ovC; i++) {
				vertexGen[i].X = baseCubeShape[i].X * scale.X;
				vertexGen[i].Y = baseCubeShape[i].Y * scale.Y;
				vertexGen[i].Z = baseCubeShape[i].Z * scale.Z;
			}

			b->id = 0x400 + idxSOBL;
			LOGD(WVRAPIWrapper, "Mesh id=%X:", b->id);
			FillSceneMeshBuffer(&(b->b), vertexGen, ovC, indicesCube, oiC);
		}

		delete[] vertexGen;
		vertexGen = nullptr;

		*listOut = SceneMeshBufferList;
		return N;
	}

	static WVR_Result GetSceneMeshBuffer(uint64_t meshBufferId, WVR_SceneMeshBuffer* sceneMeshBuffer) {
		SceneMeshBuffer* list = nullptr;
		if (sceneMeshBuffer == nullptr)
			return WVR_Error_InvalidArgument;

		bool isSceneMesh = (meshBufferId & 0x800) != 0;
		bool isSceneObject = (meshBufferId & 0x400) != 0;

		int N = PrepareDummySceneMeshBufferList(&list);
		int foundIdx = -1;
		for (int i = 0; i < N; i++) {
			if (list[i].id == meshBufferId) {
				foundIdx = i;
				break;
			}
		}

		sceneMeshBuffer->indexCountOutput = 0;
		sceneMeshBuffer->vertexCountOutput = 0;
		if (foundIdx == -1) return WVR_Error_InvalidArgument;

		const WVR_SceneMeshBuffer& found = list[foundIdx].b;

		sceneMeshBuffer->indexCountOutput = found.indexCountOutput;
		sceneMeshBuffer->vertexCountOutput = found.vertexCountOutput;
		if (!(sceneMeshBuffer->vertexBuffer != nullptr &&
			sceneMeshBuffer->indexBuffer != nullptr &&
			sceneMeshBuffer->indexCapacityInput >= found.indexCountOutput &&
			sceneMeshBuffer->vertexCapacityInput >= found.vertexCountOutput))
			return WVR_Success;

		//FVector* p = (FVector*)found.vertexBuffer;
		//for (uint32 k = 0; k < found.vertexCountOutput; k++)
		//	LOGD(WVRAPIWrapper, "[%04X] %5.3f, %5.3f, %5.3f", k, p[k].X, p[k].Y, p[k].Z);

		memcpy(sceneMeshBuffer->indexBuffer, found.indexBuffer, found.indexCountOutput * sizeof(*found.indexBuffer));
		memcpy(sceneMeshBuffer->vertexBuffer, found.vertexBuffer, found.vertexCountOutput * sizeof(*found.vertexBuffer));

		return WVR_Success;
	}


	static TMap<uint64_t, WVR_SpatialAnchorState*>& GetSpatialAnchorDummyData() {
		static TMap<uint64_t, WVR_SpatialAnchorState*> data = TMap<uint64_t, WVR_SpatialAnchorState*>();
		return data;
	}

	static TMap<FString, WVR_SpatialAnchorState*>& GetCachedSpatialAnchorDummyData() {
		static TMap<FString, WVR_SpatialAnchorState*> data = TMap<FString, WVR_SpatialAnchorState*>();
		return data;
	}

	static TMap<FString, WVR_SpatialAnchorState*>& GetPersistedSpatialAnchorDummyData() {
		static TMap<FString, WVR_SpatialAnchorState*> data = TMap<FString, WVR_SpatialAnchorState*>();
		return data;
	}

	static uint64 CreateDummyAnchor() {
		static uint64 aserialid = 0x7F00FF00FF00FF00;
		return aserialid++;
	}

	static void ToWVRAnchorName(const FString& anchorName, WVR_SpatialAnchorName& anchorNameWVR) {
		std::string str = TCHAR_TO_ANSI(*anchorName);
		int l = str.length();
		size_t m = l < WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE - 1 ? l : WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE - 1;
#if PLATFORM_ANDROID
		strncpy(anchorNameWVR.name, str.c_str(), m);
#else
		strncpy_s(anchorNameWVR.name, str.c_str(), m);
#endif
		anchorNameWVR.name[m] = 0;
	}
}  // namespace scene

static bool operator==(const WVR_Uuid& A, const WVR_Uuid& B) {
	for (int i = 0; i < WVR_UUID_SIZE; i++) {
		if (A.data[i] != B.data[i])
			return false;
	}
	return true;
}

WVR_Result FWaveVRAPIWrapper::GetScenePlanes(const WVR_ScenePlaneFilter* planeFilter, uint32_t planeCapacityInput, uint32_t* planeCountOutput, WVR_PoseOriginModel originModel, WVR_ScenePlane* planes) {
	WVR_ScenePlaneType type = planeFilter == nullptr ? WVR_ScenePlaneType_Max : planeFilter->planeType;
	WVR_ScenePlaneLabel label = planeFilter == nullptr ? WVR_ScenePlaneLabel_Max : planeFilter->planeLabel;

	WVR_ScenePlane* list = nullptr;
	int N = scene::PrepareDummyScenePlaneList(&list);
	uint32 c = 0;
	for (int i = 0; i < N; i++) {
		auto& plane = list[i];
		//LOGD(WVRAPIWrapper, "GetScenePlanes t=%u l=%u", type, label);
		if (plane.planeType == type || type == WVR_ScenePlaneType_Max) {
			//LOGD(WVRAPIWrapper, " plane.planeType=%u", type);
			if (plane.planeLabel == label || label == WVR_ScenePlaneLabel_Max) {
				//LOGD(WVRAPIWrapper, "  plane.planeLabel=%u", label);
				c++;
				// This plane will not always show. Used to test if the plane can be removed.
				if (planeCapacityInput == 0 && i == 8 && (rand() % 3 == 0)) {
					c--;
					continue;
				}
				if (c <= planeCapacityInput && planes != nullptr) {
					planes[c - 1] = plane;
					if (i == 7) {
						// This plane will have dynamic position and rotation.  Used to test if the plane ca be update.
						int offset = rand() % 4;
						planes[c - 1].pose.position.v[0] += offset / 50.0f;
						planes[c - 1].pose.rotation = planes[i - 4 + offset].pose.rotation;
					}
				}
			}
		}
	}
	if (planeCountOutput != nullptr)
		*planeCountOutput = c;
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::GetSceneObjects(uint32_t capacity, uint32_t* count, WVR_PoseOriginModel originModel, WVR_SceneObject* objects) {
	WVR_SceneObject* list = nullptr;
	int N = scene::PrepareDummySceneObjectList(&list);
	uint32 c = 0;
	for (int i = 0; i < N; i++) {
		auto& object = list[i];
		c++;
		// This object will not always show. Used to test if the plane can be removed.
		//if (capacity == 0 && i == 8 && (rand() % 3 == 0)) {
		//	c--;
		//	continue;
		//}
		if (c <= capacity && objects != nullptr) {
			objects[c - 1] = object;
			//if (i == 7) {
			//	// This object will have dynamic position and rotation.  Used to test if the object ca be update.
			//	int offset = rand() % 4;
			//	objects[c - 1].pose.position.v[0] += offset / 50.0f;
			//	objects[c - 1].pose.rotation = objects[i - 4 + offset].pose.rotation;
			//}
		}
	}
	if (count != nullptr)
		*count = c;
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::GetSceneMeshes(WVR_SceneMeshType type, uint32_t meshCapacityInput, uint32_t* meshCountOutput, WVR_SceneMesh* meshes) {
	if (meshCountOutput == nullptr) return WVR_Error_InvalidArgument;
	switch (type) {
	case WVR_SceneMeshType_VisualMesh:
	case WVR_SceneMeshType_ColliderMesh:
	{
		scene::SceneMeshBuffer* list = nullptr;
		uint32 ca = scene::PrepareDummySceneMeshBufferList(&list);
		uint32 c = 0;
		for (uint32 i = 0; i < ca; i++) {
			if ((list[i].id & 0x800) > 0)
				c++;
		}
		TArray<uint32_t> drops;
		uint32 randC = 0;
		if (c != 0)
			randC = (std::rand() % 5) + (c - 4);
		if (meshCapacityInput == 0) {
			*meshCountOutput = randC;
		} else {
			*meshCountOutput = meshCapacityInput;
		}

		if (meshes == nullptr || meshCapacityInput == 0)
			return WVR_Success;
		int count = 0;
		for (uint32 i = 0; i < c; i++) {
			if ((meshCapacityInput == 5 && (i == 1 || i == 3 || i == 5 || i == 7)) ||
				(meshCapacityInput == 6 && (i == 0 || i == 2 || i == 7)) ||
				(meshCapacityInput == 7 && (i == 3 || i == 5)) ||
				(meshCapacityInput == 8 && i == 4))
				continue;
			meshes[count++].meshBufferId = 0x800 + i;
		}
		return WVR_Success;
	}
	default:
		*meshCountOutput = 0;
		return WVR_Success;
	}
	return WVR_Error_Data_Invalid;
}


WVR_Result FWaveVRAPIWrapper::GetSceneMeshBuffer(uint64_t meshBufferId, WVR_PoseOriginModel originModel, WVR_SceneMeshBuffer* sceneMeshBuffer) {
	if (scene::GetScenePlaneMeshBuffer(meshBufferId, sceneMeshBuffer) == WVR_Success)
		return WVR_Success;
	else if (scene::GetSceneMeshBuffer(meshBufferId, sceneMeshBuffer) == WVR_Success)
		return WVR_Success;
	return WVR_Error_Data_Invalid;
}

WVR_Result FWaveVRAPIWrapper::CreateSpatialAnchor(const WVR_SpatialAnchorCreateInfo* createInfo, WVR_SpatialAnchor* anchor) {
	auto& dummy = scene::GetSpatialAnchorDummyData();
	if (createInfo == nullptr)
		return WVR_Error_InvalidArgument;
	WVR_SpatialAnchorState* state = new WVR_SpatialAnchorState();
	state->pose = createInfo->pose;
	state->trackingState = WVR_SpatialAnchorTrackingState_Tracking;
	memcpy(state->anchorName.name, createInfo->anchorName.name, WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE);
	uint64 a = scene::CreateDummyAnchor();
	dummy.Add(a, state);
	*anchor = a;

	LOGD(WVRAPIWrapper, "CreateSpatialAnchor %016" PRIX64, *anchor);
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::DestroySpatialAnchor(WVR_SpatialAnchor anchor) {
	LOGD(WVRAPIWrapper, "DestroySpatialAnchor+ %016" PRIX64, anchor);
	auto& dummy = scene::GetSpatialAnchorDummyData();
	//if (!dummy.Contains(anchor)) {
	//	LOGD(WVRAPIWrapper, "!dummy.Contains");
	//	return WVR_Error_Data_Invalid;
	//}

	WVR_SpatialAnchorState** state = dummy.Find(anchor);
	if (state == nullptr) {
		LOGE(WVRAPIWrapper, "state == nullptr");
		return WVR_Error_Data_Invalid;
	}

	if (*state != nullptr)
		delete (*state);

	dummy.Remove(anchor);
	LOGD(WVRAPIWrapper, "DestroySpatialAnchor-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::EnumerateSpatialAnchors(uint32_t anchorCapacityInput, uint32_t* anchorCountOutput, WVR_SpatialAnchor* anchors) {
	LOGD(WVRAPIWrapper, "EnumerateSpatialAnchors+");
	std::srand(time(NULL));
	auto& dummy = scene::GetSpatialAnchorDummyData();
	if (anchorCapacityInput != 0 && anchors == nullptr || anchorCountOutput == nullptr) {
		LOGE(WVRAPIWrapper, "Invalid output space.");
		return WVR_Error_InvalidArgument;
	}
	uint32 N = dummy.Num();
	*anchorCountOutput = N;
	if (anchorCapacityInput < N)
		return WVR_Success;

	uint32 i = 0;
	for (auto itor = dummy.begin(); itor != dummy.end() && i < N; ++itor, ++i) {
		anchors[i] = itor.Key();
		LOGD(WVRAPIWrapper, "%u: %016" PRIX64, i, anchors[i]);
	}
	LOGD(WVRAPIWrapper, "EnumerateSpatialAnchors-");

	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::GetSpatialAnchorState(WVR_SpatialAnchor anchor, WVR_PoseOriginModel origin, WVR_SpatialAnchorState* anchorState) {
	if (anchorState == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummy = scene::GetSpatialAnchorDummyData();
	WVR_SpatialAnchorState** statePtr = dummy.Find(anchor);
	if (statePtr == nullptr || *statePtr == nullptr)
		return WVR_Error_Data_Invalid;
	*anchorState = **statePtr;
	const float x = *anchorState->pose.position.v;
	const float y = *(anchorState->pose.position.v + 1);
	const float z = *(anchorState->pose.position.v + 2);
	const float i = anchorState->pose.rotation.w;
	const float j = anchorState->pose.rotation.x;
	const float k = anchorState->pose.rotation.y;
	const float l = anchorState->pose.rotation.z;
	int tsr = std::rand() % 10;
	int ts = 0;
	if (tsr >= 7)  // 20%
		ts = 1;
	if (tsr >= 9)  // 10%
		ts = 2;

	anchorState->trackingState = (WVR_SpatialAnchorTrackingState)ts;
	//LOGD(WVRAPIWrapper, "GetSpatialAnchorState: %016" PRIX64 ", n=%s, p=(%f, %f, %f), r=(w=%f, %f, %f, %f) ts=%d", anchor, anchorState->anchorName.name, x, y, z, i, j, k, l, ts);
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::CacheSpatialAnchor(const WVR_SpatialAnchorCacheInfo* spatialAnchorCacheInfo) {
	LOGD(WVRAPIWrapper, "CacheSpatialAnchor+");
	if (spatialAnchorCacheInfo == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyC = scene::GetCachedSpatialAnchorDummyData();
	auto name = FString(spatialAnchorCacheInfo->cachedSpatialAnchorName.name);
	WVR_SpatialAnchorState ** stateCPtr = dummyC.Find(name);
	if (stateCPtr != nullptr) {
		LOGE(WVRAPIWrapper, "Cached Anchor name duplicated.");
		return WVR_Error_Anchor_NameDuplicated;
	}
	auto& dummy = scene::GetSpatialAnchorDummyData();
	WVR_SpatialAnchorState** statePtr = dummy.Find(spatialAnchorCacheInfo->spatialAnchor);
	if (statePtr == nullptr || *statePtr == nullptr) {
		LOGE(WVRAPIWrapper, "Anchor not exist.");
		return WVR_Error_InvalidArgument;
	}

	WVR_SpatialAnchorState* stateCPtr2 = new WVR_SpatialAnchorState();
	*stateCPtr2 = **statePtr;
	stateCPtr2->trackingState = WVR_SpatialAnchorTrackingState::WVR_SpatialAnchorTrackingState_Stopped;
	dummyC.Add(name, stateCPtr2);
	LOGD(WVRAPIWrapper, "CacheSpatialAnchor-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::UncacheSpatialAnchor(const WVR_SpatialAnchorName* cachedSpatialAnchorName) {
	LOGD(WVRAPIWrapper, "UncacheSpatialAnchor+");
	if (cachedSpatialAnchorName == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyC = scene::GetCachedSpatialAnchorDummyData();
	auto name = FString(cachedSpatialAnchorName->name);

	WVR_SpatialAnchorState** stateCPtr = dummyC.Find(name);
	if (stateCPtr == nullptr || *stateCPtr == nullptr) {
		LOGE(WVRAPIWrapper, "The cached anchor %s is not exist", TCHAR_TO_ANSI(*name));
		return WVR_Error_Data_Invalid;
	}

	delete *stateCPtr;
	dummyC.Remove(name);
	LOGD(WVRAPIWrapper, "UncacheSpatialAnchor-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::EnumerateCachedSpatialAnchorNames(uint32_t cachedSpatialAnchorNamesCapacityInput, uint32_t* cachedSpatialAnchorNamesCountOutput, WVR_SpatialAnchorName* cachedSpatialAnchorNames) {
	LOGD(WVRAPIWrapper, "EnumerateCachedSpatialAnchorNames+");
	if (cachedSpatialAnchorNamesCountOutput == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyC = scene::GetCachedSpatialAnchorDummyData();
	uint32 N = dummyC.Num();
	*cachedSpatialAnchorNamesCountOutput = N;
	if (cachedSpatialAnchorNamesCapacityInput < N)
		return WVR_Success;

	uint32 i = 0;
	for (auto itor = dummyC.CreateConstIterator(); itor; ++itor, ++i) {
		cachedSpatialAnchorNames[i] = WVR_SpatialAnchorName();
		scene::ToWVRAnchorName(itor.Key(), cachedSpatialAnchorNames[i]);
		LOGD(WVRAPIWrapper, "%u: %s", i, TCHAR_TO_ANSI(*itor.Key()));
	}
	LOGD(WVRAPIWrapper, "EnumerateCachedSpatialAnchorNames-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::ClearCachedSpatialAnchors() {
	LOGD(WVRAPIWrapper, "ClearCachedSpatialAnchors+");
	auto& dummyC = scene::GetCachedSpatialAnchorDummyData();
	for (auto itor = dummyC.CreateConstIterator(); itor; ++itor) {
		delete itor.Value();
	}
	dummyC.Empty();
	LOGD(WVRAPIWrapper, "ClearCachedSpatialAnchors-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::CreateSpatialAnchorFromCacheName(const WVR_SpatialAnchorFromCacheNameCreateInfo* createInfo, WVR_SpatialAnchor* anchor) {
	LOGD(WVRAPIWrapper, "CreateSpatialAnchorFromCacheName+");
	if (createInfo == nullptr || anchor == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyC = scene::GetCachedSpatialAnchorDummyData();
	auto name = FString(createInfo->cachedSpatialAnchorName.name);
	WVR_SpatialAnchorState** stateCPtr = dummyC.Find(name);
	if (stateCPtr == nullptr || *stateCPtr == nullptr) {
		LOGE(WVRAPIWrapper, "The cached anchor %s is not exist", TCHAR_TO_ANSI(*name));
		return WVR_Error_Data_Invalid;
	}

	auto& dummy = scene::GetSpatialAnchorDummyData();
	WVR_SpatialAnchorState* statePtr = new WVR_SpatialAnchorState();
	statePtr->anchorName = createInfo->spatialAnchorName;
	statePtr->trackingState = WVR_SpatialAnchorTrackingState::WVR_SpatialAnchorTrackingState_Stopped;
	statePtr->pose = (*stateCPtr)->pose;
	uint64 a = scene::CreateDummyAnchor();
	dummy.Add(a, statePtr);

	*anchor = a;
	LOGD(WVRAPIWrapper, "CreateSpatialAnchorFromCacheName-");
	return WVR_Success;

}

WVR_Result FWaveVRAPIWrapper::PersistSpatialAnchor(const WVR_SpatialAnchorPersistInfo* spatialAnchorPersistInfo) {
	LOGD(WVRAPIWrapper, "PersistSpatialAnchor+");
	if (spatialAnchorPersistInfo == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	auto name = FString(spatialAnchorPersistInfo->persistedSpatialAnchorName.name);
	WVR_SpatialAnchorState** statePPtr = dummyP.Find(name);
	if (statePPtr != nullptr) {
		LOGE(WVRAPIWrapper, "Persisted Anchor name duplicated.");
		return WVR_Error_Anchor_NameDuplicated;
	}
	auto& dummy = scene::GetSpatialAnchorDummyData();
	WVR_SpatialAnchorState** statePtr = dummy.Find(spatialAnchorPersistInfo->spatialAnchor);
	if (statePtr == nullptr || *statePtr == nullptr) {
		LOGE(WVRAPIWrapper, "Anchor not exist.");
		return WVR_Error_InvalidArgument;
	}

	WVR_SpatialAnchorState* statePPtr2 = new WVR_SpatialAnchorState();
	*statePPtr2 = **statePtr;
	statePPtr2->trackingState = WVR_SpatialAnchorTrackingState::WVR_SpatialAnchorTrackingState_Stopped;
	dummyP.Add(name, statePPtr2);
	LOGD(WVRAPIWrapper, "PersistSpatialAnchor-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::UnpersistSpatialAnchor(const WVR_SpatialAnchorName* persistedSpatialAnchorName) {
	LOGD(WVRAPIWrapper, "UnpersistSpatialAnchor+");
	if (persistedSpatialAnchorName == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	auto name = FString(persistedSpatialAnchorName->name);
	WVR_SpatialAnchorState** statePPtr = dummyP.Find(name);
	if (statePPtr == nullptr || *statePPtr == nullptr) {
		LOGE(WVRAPIWrapper, "Persisted Anchor not exist.");
		return WVR_Error_InvalidArgument;
	}

	delete *statePPtr;
	dummyP.Remove(name);
	LOGD(WVRAPIWrapper, "UnpersistSpatialAnchor-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::EnumeratePersistedSpatialAnchorNames(uint32_t persistedSpatialAnchorNamesCapacityInput, uint32_t* persistedSpatialAnchorNamesCountOutput, WVR_SpatialAnchorName* persistedSpatialAnchorNames) {
	LOGD(WVRAPIWrapper, "EnumeratePersistedSpatialAnchorNames+");
	if (persistedSpatialAnchorNamesCountOutput == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	uint32 N = dummyP.Num();
	*persistedSpatialAnchorNamesCountOutput = N;
	if (persistedSpatialAnchorNamesCapacityInput < N)
		return WVR_Success;

	uint32 i = 0;
	for (auto itor = dummyP.CreateConstIterator(); itor; ++itor, ++i) {
		persistedSpatialAnchorNames[i] = WVR_SpatialAnchorName();
		scene::ToWVRAnchorName(itor.Key(), persistedSpatialAnchorNames[i]);
		LOGD(WVRAPIWrapper, "%u: %s", i, TCHAR_TO_ANSI(*itor.Key()));
	}
	LOGD(WVRAPIWrapper, "EnumeratePersistedSpatialAnchorNames-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::ClearPersistedSpatialAnchors() {
	LOGD(WVRAPIWrapper, "ClearPersistedSpatialAnchors+");
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	for (auto itor = dummyP.CreateConstIterator(); itor; ++itor) {
		delete itor.Value();
	}
	dummyP.Empty();
	LOGD(WVRAPIWrapper, "ClearPersistedSpatialAnchors-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::GetPersistedSpatialAnchorCount(WVR_PersistedSpatialAnchorCountGetInfo* getInfo) {
	LOGD(WVRAPIWrapper, "GetPersistedSpatialAnchorCount+");
	if (getInfo == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	getInfo->currentTrackingCount = dummyP.Num();
	getInfo->maximumTrackingCount = 1981;
	LOGD(WVRAPIWrapper, "GetPersistedSpatialAnchorCount-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::CreateSpatialAnchorFromPersistenceName(const WVR_SpatialAnchorFromPersistenceNameCreateInfo* createInfo, WVR_SpatialAnchor* anchor) {
	LOGD(WVRAPIWrapper, "CreateSpatialAnchorFromPersistenceName+");
	if (createInfo == nullptr || anchor == nullptr)
		return WVR_Error_InvalidArgument;
	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	auto name = FString(createInfo->persistedSpatialAnchorName.name);
	WVR_SpatialAnchorState** statePPtr = dummyP.Find(name);
	if (statePPtr == nullptr || *statePPtr == nullptr) {
		LOGE(WVRAPIWrapper, "Persisted Anchor not exist.");
		return WVR_Error_InvalidArgument;
	}

	auto& dummy = scene::GetSpatialAnchorDummyData();
	WVR_SpatialAnchorState* statePtr = new WVR_SpatialAnchorState();

	statePtr->anchorName = createInfo->spatialAnchorName;
	statePtr->trackingState = WVR_SpatialAnchorTrackingState::WVR_SpatialAnchorTrackingState_Stopped;
	statePtr->pose = (*statePPtr)->pose;
	uint64 a = scene::CreateDummyAnchor();
	dummy.Add(a, statePtr);
	*anchor = a;

	LOGD(WVRAPIWrapper, "CreateSpatialAnchorFromPersistenceName-");
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::ExportPersistedSpatialAnchor(const WVR_SpatialAnchorName* persistedSpatialAnchorName, uint32_t dataCapacityInput, uint32_t* dataCountOutput, char* data) {

	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	auto name = FString(persistedSpatialAnchorName->name);
	WVR_SpatialAnchorState** statePPtr = dummyP.Find(name);
	if (statePPtr == nullptr || *statePPtr == nullptr) {
		LOGE(WVRAPIWrapper, "Persisted Anchor not exist.");
		return WVR_Error_InvalidArgument;
	}

	if (dataCountOutput == nullptr)
	{
		LOGE(WVRAPIWrapper, "dataCountOutput is nullptr");
		return WVR_Error_InvalidArgument;
	}
	*dataCountOutput = WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE;
	if (dataCapacityInput < WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE) {
		return WVR_Success;
	}

	*dataCountOutput = WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE;
	memcpy(data, persistedSpatialAnchorName->name, WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE);
	return WVR_Success;
}

WVR_Result FWaveVRAPIWrapper::ImportPersistedSpatialAnchor(uint32_t dataCount, const char* data) {
	if (data == nullptr)
	{
		LOGE(WVRAPIWrapper, "data is nullptr");
		return WVR_Error_InvalidArgument;
	}

	auto& dummyP = scene::GetPersistedSpatialAnchorDummyData();
	if (dummyP.Num() >= 1981) {
		LOGE(WVRAPIWrapper, "Persisted Anchor is full.");
		return WVR_Error_InvalidArgument;
	}

	if (dataCount > WVR_MAX_SPATIAL_ANCHOR_NAME_SIZE) {
		LOGE(WVRAPIWrapper, "dataCount is too large.");
		return WVR_Error_InvalidArgument;
	}

	FString name = FString(ANSI_TO_TCHAR(data));
	if (name.IsEmpty()) {
		LOGE(WVRAPIWrapper, "data is empty.");
		return WVR_Error_InvalidArgument;
	}

	if (dummyP.Find(name) != nullptr) {
		LOGE(WVRAPIWrapper, "Persisted Anchor already exist.");
		return WVR_Error_InvalidArgument;
	}

	WVR_SpatialAnchorState* statePtr = new WVR_SpatialAnchorState();
	scene::ToWVRAnchorName(name, statePtr->anchorName);
	statePtr->trackingState = WVR_SpatialAnchorTrackingState::WVR_SpatialAnchorTrackingState_Stopped;
	statePtr->pose.position.v[0] = 0;
	statePtr->pose.position.v[1] = 0;
	statePtr->pose.position.v[2] = 0;
	statePtr->pose.rotation.w = 1;
	statePtr->pose.rotation.x = 0;
	statePtr->pose.rotation.y = 0;
	statePtr->pose.rotation.z = 0;

	dummyP.Add(name, statePtr);
	LOGD(WVRAPIWrapper, "ImportPersistedSpatialAnchor-");
	return WVR_Success;
}


namespace marker {
	struct FakeArUcoMarker {
		WVR_ArucoMarker marker;
		bool isTracked;
	};

	struct FakeMarker {
		WVR_TrackableMarkerState ta;
	};

	struct MarkerFakeData {
		WVR_MarkerObserverTarget target;
		bool mkIsStart;
		bool obIsStart;
		bool obDetIsStart;

		TArray<WVR_Uuid> startTrackedMarkers;
	};

	static MarkerFakeData fakeData;
	static void StrnCpy(char* dst, size_t size, const char* src) {
		if (dst == nullptr || src == nullptr)
			return;
		int l = strlen(src);
		size_t m = l < WVR_MAX_MARKER_NAME_SIZE - 1 ? l : WVR_MAX_MARKER_NAME_SIZE - 1;
#if PLATFORM_ANDROID
		strncpy(dst, src, size);
		dst[l] = 0;
#else
		strncpy_s(dst, size, src, m);
#endif
	}

#ifndef MAKE_ARUCO_MARKER
#define MAKE_ARUCO_MARKER(_am, tid, _size, pos, rot, _name) \
do { \
	auto& am = (_am); \
	scene::GenerateUUID(am.uuid.data, tid, false); \
	am.trackerId = tid; \
	am.size = _size; \
	am.state = WVR_MarkerTrackingState_Detected; \
	am.pose.position = scene::pos; \
	am.pose.rotation = scene::rot; \
	marker::StrnCpy(am.markerName.name, WVR_MAX_MARKER_NAME_SIZE, _name); \
} while(false)
#endif

	static uint32 PrepareDummyArUcoMarkerList(WVR_ArucoMarker** listOut = nullptr) {
		static WVR_ArucoMarker* WaveDummyArucoMarkerList = nullptr;
		LOGD(WVRAPIWrapper, "PrepareDummyArUcoMarkerList list=%p", WaveDummyArucoMarkerList);
		const int N = 9;
		if (WaveDummyArucoMarkerList != nullptr) {
			if (listOut != nullptr)
				*listOut = WaveDummyArucoMarkerList;
			return N;
		}
		WaveDummyArucoMarkerList = new WVR_ArucoMarker[N];
		auto& list = WaveDummyArucoMarkerList;
		const float ND = 0.7071068f;
		const float W = 8;
		const float H = 4;
		const float D = 6;
		std::srand(time(NULL));
		MAKE_ARUCO_MARKER(list[0], 0, 0.1f, V3(0, 0.8f, -0.45f), Q4(-ND, 0, 0, ND), "Desktop");
		MAKE_ARUCO_MARKER(list[1], 1, 0.4f, V3(0, 0.01f, 0), Q4(-ND, 0, 0, ND), "Floor");
		MAKE_ARUCO_MARKER(list[2], 2, 0.4f, V3(0, H, 0), Q4(ND, 0, 0, ND), "Celing");
		MAKE_ARUCO_MARKER(list[3], 3, 0.4f, V3(0, H / 2, -D / 2), Q4(0, 0, 0, 1), "Front Wall");
		MAKE_ARUCO_MARKER(list[4], 4, 0.4f, V3(W / 2, H / 2, 0), Q4(0, ND, 0, ND), "Right Wall");
		MAKE_ARUCO_MARKER(list[5], 5, 0.4f, V3(-W / 2, H / 2, 0), Q4(0, -ND, 0, ND), "Left Wall");
		MAKE_ARUCO_MARKER(list[6], 6, 0.4f, V3(0, H / 2, D / 2), Q4(0, 1, 0, 0), "Rear Wall");
		MAKE_ARUCO_MARKER(list[7], 7, 0.1f, V3(-0.5, 0.8f, -0.5f), Q4(0, 0, 0, 1), "Moveable");
		MAKE_ARUCO_MARKER(list[8], 8, 0.1f, V3(-0.5, 1.0f, -0.5f), Q4(-ND, 0, 0, ND), "Show/Hide");

		if (listOut == nullptr)
			return 0;
		*listOut = WaveDummyArucoMarkerList;
		return N;
	}
	struct MyTrackableData {
		WVR_Uuid uuid;
		WVR_TrackableMarkerState state;
		WVR_ArucoMarker* aruco;
	};

	static TArray<MyTrackableData>& GetMarkerTrackableDummyData(bool reset = false) {
		static TArray<MyTrackableData> map = TArray<MyTrackableData>();
		if (reset) map.Empty();
		return map;
	}

	WVR_ArucoMarker* FindArUcoMaker(WVR_Uuid uuid) {
		WVR_ArucoMarker* list = nullptr;
		const uint32_t n = marker::PrepareDummyArUcoMarkerList(&list);
		if (list == nullptr) return nullptr;
		for (uint32_t i = 0; i < n; i++) {
			if (list[i].uuid == uuid)
				return list + i;
		}
		return nullptr;
	}
}

WVR_Result FWaveVRAPIWrapper::StartMarker() {
	marker::fakeData.mkIsStart = true;
	marker::PrepareDummyArUcoMarkerList(nullptr);
	marker::GetMarkerTrackableDummyData(true);
	return WVR_Success;
}
void FWaveVRAPIWrapper::StopMarker() { marker::fakeData.mkIsStart = false;  }
WVR_Result FWaveVRAPIWrapper::StartMarkerObserver(WVR_MarkerObserverTarget target) {
	if (!marker::fakeData.mkIsStart)
		return WVR_Error_InvalidArgument;
	marker::fakeData.target = target;
	marker::fakeData.obIsStart = true;
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::StopMarkerObserver(WVR_MarkerObserverTarget target) {
	if (!marker::fakeData.obIsStart || marker::fakeData.target != target)
		return WVR_Error_InvalidArgument;
	marker::fakeData.obIsStart = false;
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::GetMarkerObserverState(
	WVR_MarkerObserverTarget target,
	WVR_MarkerObserverState* state) {
	if (!marker::fakeData.obIsStart || marker::fakeData.target != target)
		return WVR_Error_InvalidArgument;
	if (marker::fakeData.obDetIsStart)
		*state = WVR_MarkerObserverState::WVR_MarkerObserverState_Detecting;
	else if (marker::fakeData.startTrackedMarkers.Num() > 0)
		*state = WVR_MarkerObserverState::WVR_MarkerObserverState_Tracking;
	else
		*state = WVR_MarkerObserverState::WVR_MarkerObserverState_Idle;
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::StartMarkerDetection(WVR_MarkerObserverTarget target) {
	if (!marker::fakeData.obIsStart || marker::fakeData.target != target)
		return WVR_Error_InvalidArgument;
	marker::fakeData.obDetIsStart = true;
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::StopMarkerDetection(WVR_MarkerObserverTarget target) {
	if (!marker::fakeData.obDetIsStart || marker::fakeData.target != target)
		return WVR_Error_InvalidArgument;
	marker::fakeData.obDetIsStart = false;
	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::GetArucoMarkers(uint32_t markerCapacityInput, uint32_t* markerCountOutput, WVR_PoseOriginModel originModel, WVR_ArucoMarker* markers) {
	// String in UE5 didn't support %p
	LOGD(WVRAPIWrapper, "GetArucoMarkers ci=%u, ori=%d", markerCapacityInput, originModel);
	LOGD(WVRAPIWrapper, "GetArucoMarkers mk=%u, ob=%d, obd=%d", marker::fakeData.mkIsStart, marker::fakeData.obIsStart, marker::fakeData.obDetIsStart);

	bool needCount = markerCountOutput != nullptr;
	bool needData = markers != nullptr || markerCapacityInput > 0;

	if (!marker::fakeData.obIsStart) {
		if (needCount) *markerCountOutput = 0;
		return WVR_Error_SystemInvalid;
	}

	WVR_ArucoMarker* list = nullptr;
	const uint32_t n = marker::PrepareDummyArUcoMarkerList(&list);
	if (list == nullptr) return WVR_Error_Data_Invalid;
	
	if (marker::fakeData.obDetIsStart) {
		if (needCount) *markerCountOutput = n;
		if (needData) {
			for (uint32_t i = 0; i < n && i < markerCapacityInput; i++) {
				markers[i] = list[i];
				markers[i].state = WVR_MarkerTrackingState_Detected;
			}
		}
	} else {
		auto& map = marker::GetMarkerTrackableDummyData();
		for (uint32 i = 0; i < (uint32)map.Num() && i < markerCapacityInput; i++) {
			if (needData) {
				markers[i] = *(map[i].aruco);
				bool startTracked = marker::fakeData.startTrackedMarkers.Contains(markers[i].uuid);
				WVR_MarkerTrackingState trackingState = startTracked ? WVR_MarkerTrackingState_Tracked : WVR_MarkerTrackingState_Stopped;
				markers[i].state = trackingState;
			}
		}
		if (needCount) *markerCountOutput = map.Num();
	}

	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::EnumerateTrackableMarkers(WVR_MarkerObserverTarget target, uint32_t markerCapacityInput, uint32_t* markerCountOutput, WVR_Uuid* uuids) {
	if (marker::fakeData.target != target)
		return WVR_Error_InvalidArgument;

	bool needCount = markerCountOutput != nullptr;
	bool needData = uuids != nullptr && markerCapacityInput > 0;

	if (!marker::fakeData.obIsStart) {
		if (needCount) *markerCountOutput = 0;
		return WVR_Error_SystemInvalid;
	}

	auto& map = marker::GetMarkerTrackableDummyData();
	if (needData) {
		for (uint32 i = 0; i < (uint32)map.Num() && i < markerCapacityInput; i++)
			uuids[i] = map[i].aruco->uuid;
	}
	if (needCount) *markerCountOutput = map.Num();

	return WVR_Success;
}
WVR_Result FWaveVRAPIWrapper::CreateTrackableMarker(const WVR_TrackableMarkerCreateInfo* createInfo) {
	auto uuid = createInfo->uuid;
	WVR_ArucoMarker* list = nullptr;
	const uint32_t n = marker::PrepareDummyArUcoMarkerList(&list);
	if (list == nullptr) return WVR_Error_Data_Invalid;

	auto& map = marker::GetMarkerTrackableDummyData();
	for (int i = 0; i < map.Num(); i++) {
		int index = map[i].aruco->trackerId;
		if (list[index].uuid == uuid)
			return WVR_Success;
	}

	for (uint32 i = 0; i < n; i++) {
		if (list[i].uuid == uuid) {
			marker::MyTrackableData data;
			data.uuid = uuid;
			data.state.state = WVR_MarkerTrackingState_Stopped;
			data.state.target = WVR_MarkerObserverTarget_Aruco;
			data.state.pose = list[i].pose;
			data.state.markerName = createInfo->markerName;
			marker::StrnCpy(data.state.markerName.name, WVR_MAX_MARKER_NAME_SIZE, createInfo->markerName.name);
			data.aruco = marker::FindArUcoMaker(uuid);  // XXX must have, or crash...
			map.Add(data);
			return WVR_Success;
		}
	}
	return WVR_Error_Data_Invalid;
}
WVR_Result FWaveVRAPIWrapper::DestroyTrackableMarker(WVR_Uuid uuid) {
	auto& map = marker::GetMarkerTrackableDummyData();
	for (int i = 0; i < map.Num(); i++) {
		if (map[i].uuid == uuid) {
			map.RemoveAt(i);
			return WVR_Success;
		}
	}
	return WVR_Error_Data_Invalid;
}
WVR_Result FWaveVRAPIWrapper::StartTrackableMarkerTracking(WVR_Uuid uuid) {
	if (marker::fakeData.startTrackedMarkers.Contains(uuid))
		return WVR_Success;
	auto& map = marker::GetMarkerTrackableDummyData();
	for (int i = 0; i < map.Num(); i++) {
		if (map[i].uuid == uuid) {
			marker::fakeData.startTrackedMarkers.Add(uuid);
			return WVR_Success;
		}
	}
	return WVR_Error_Data_Invalid;
}
WVR_Result FWaveVRAPIWrapper::StopTrackableMarkerTracking(WVR_Uuid uuid) {
	if (marker::fakeData.startTrackedMarkers.Contains(uuid)) {
		marker::fakeData.startTrackedMarkers.Remove(uuid);
		return WVR_Success;
	}
	return WVR_Error_Data_Invalid;
}
WVR_Result FWaveVRAPIWrapper::GetTrackableMarkerState(WVR_Uuid uuid, WVR_PoseOriginModel originModel, WVR_TrackableMarkerState* state) {
	auto& map = marker::GetMarkerTrackableDummyData();
	for (int i = 0; i < map.Num(); i++) {
		if (map[i].uuid == uuid) {
			bool startTracked = marker::fakeData.startTrackedMarkers.Contains(uuid);
			WVR_MarkerTrackingState trackingState = startTracked ? WVR_MarkerTrackingState_Tracked : WVR_MarkerTrackingState_Stopped;
			map[i].state.state = trackingState;
			*state = map[i].state;
			return WVR_Success;
		}
	}
	return WVR_Error_Data_Invalid;
}

WVR_Result FWaveVRAPIWrapper::GetArucoMarkerData(WVR_Uuid uuid, WVR_ArucoMarkerData* data) {
	auto& map = marker::GetMarkerTrackableDummyData();
	for (int i = 0; i < map.Num(); i++) {
		if (map[i].uuid == uuid) {
			data->trackerId = map[i].aruco->trackerId;
			data->size = map[i].aruco->size;
			return WVR_Success;
		}
	}

	return WVR_Error_Data_Invalid;
}

WVR_Result FWaveVRAPIWrapper::ClearTrackableMarkers() {
	return WVR_Error_Data_Invalid;
}

