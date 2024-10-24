// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Hand/CustomGesture/WaveVRCustomGesture.h"
#include <cmath>
#include "Math/Quat.h"
#include "Math/UnrealMathUtility.h"

#include "Platforms/WaveVRLogWrapper.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRCustomGesture, Log, All);

// Sets default values
AWaveVRCustomGesture::AWaveVRCustomGesture()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	s_JointPositionsLeft.Init(FVector::ZeroVector, EWaveVRHandJointCount); // count of WVR_HandJoint
	s_JointPositionsRight.Init(FVector::ZeroVector, EWaveVRHandJointCount); // count of WVR_HandJoint
	s_JointRotationsLeft.Init(FQuat::Identity, EWaveVRHandJointCount); // count of WVR_HandJoint
	s_JointRotationsRight.Init(FQuat::Identity, EWaveVRHandJointCount); // count of WVR_HandJoint
}

// Called when the game starts or when spawned
void AWaveVRCustomGesture::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaveVRCustomGesture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	validPoseLeft = UWaveVRHandBPLibrary::GetAllHandJointPoses(EWaveVRTrackerType::Natural, EWaveVRHandType::Left, s_JointPositionsLeft, s_JointRotationsLeft);
	validPoseRight = UWaveVRHandBPLibrary::GetAllHandJointPoses(EWaveVRTrackerType::Natural, EWaveVRHandType::Right, s_JointPositionsRight, s_JointRotationsRight);

	/// Updates all fingers' states.
	UpdateFingerState();

	/// Checks left gestures.
	bool matchLeft = false;
	for (int i = 0; i < LeftGestures.Num(); i++)
	{
		if (MatchGestureSingle(LeftGestures[i].Setting, true))
		{
			if (!m_LeftHandGesture.Equals(LeftGestures[i].Name))
			{
				m_LeftHandGesture = LeftGestures[i].Name;
				LOGD(LogWaveVRCustomGesture, "Tick() broadcast Left custome gesture %s", PLATFORM_CHAR((*m_LeftHandGesture)));
				UWaveVRHandGestureComponent::OnCustomGestureNative_Left.Broadcast(m_LeftHandGesture);
			}

			matchLeft = true;
			break; // Leaves if matched gesture.
		}
	}
	if (!matchLeft && !m_LeftHandGesture.Equals(kUnknownGesture))
	{
		m_LeftHandGesture = kUnknownGesture;
		LOGD(LogWaveVRCustomGesture, "Tick() broadcast Left custome gesture %s", PLATFORM_CHAR((*m_LeftHandGesture)));
		UWaveVRHandGestureComponent::OnCustomGestureNative_Left.Broadcast(m_LeftHandGesture);
	}

	/// Checks right gestures.
	bool matchRight = false;
	for (int i = 0; i < RightGestures.Num(); i++)
	{
		if (MatchGestureSingle(RightGestures[i].Setting, false))
		{
			if (!m_RightHandGesture.Equals(RightGestures[i].Name))
			{
				m_RightHandGesture = RightGestures[i].Name;
				LOGD(LogWaveVRCustomGesture, "Tick() broadcast Right custome gesture %s", PLATFORM_CHAR((*m_RightHandGesture)));
				UWaveVRHandGestureComponent::OnCustomGestureNative_Right.Broadcast(m_RightHandGesture);
			}

			matchRight = true;
			break; // Leaves if matched gesture.
		}
	}
	if (!matchRight && !m_RightHandGesture.Equals(kUnknownGesture))
	{
		m_RightHandGesture = kUnknownGesture;
		LOGD(LogWaveVRCustomGesture, "Tick() broadcast Right custome gesture %s", PLATFORM_CHAR((*m_RightHandGesture)));
		UWaveVRHandGestureComponent::OnCustomGestureNative_Right.Broadcast(m_RightHandGesture);
	}

	bool matchDual = false;
	for (int i = 0; i < DualHandGestures.Num(); i++)
	{
		if (MatchGestureDual(DualHandGestures[i].Setting))
		{
			if (!m_DualHandGesture.Equals(DualHandGestures[i].Name))
			{
				m_DualHandGesture = DualHandGestures[i].Name;
				LOGD(LogWaveVRCustomGesture, "Tick() broadcast Dual Hand custome gesture %s", PLATFORM_CHAR((*m_DualHandGesture)));
				UWaveVRHandGestureComponent::OnCustomGestureNative_Dual.Broadcast(m_DualHandGesture);
			}

			matchDual = true;
			break; // Leaves if matched gesture.
		}
	}

	if (!matchDual && !m_DualHandGesture.Equals(kUnknownGesture))
	{
		m_DualHandGesture = kUnknownGesture;
		LOGD(LogWaveVRCustomGesture, "Tick() broadcast Dual Hand custome gesture %s", PLATFORM_CHAR((*m_DualHandGesture)));
		UWaveVRHandGestureComponent::OnCustomGestureNative_Dual.Broadcast(m_DualHandGesture);
	}
}

void AWaveVRCustomGesture::UpdateFingerState()
{
	if (validPoseLeft)
	{
		m_ThumbStateLeft = GetThumbState(
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Thumb_Joint1],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Thumb_Joint2],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Thumb_Tip],
			true
		);

		m_IndexStateLeft = GetFingerState(
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Index_Joint1],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Index_Joint2],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Index_Tip],
			true
		);

		m_MiddleStateLeft = GetFingerState(
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Middle_Joint1],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Middle_Joint2],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Middle_Tip],
			true
		);

		m_RingStateLeft = GetFingerState(
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Ring_Joint1],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Ring_Joint2],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Ring_Tip],
			true
		);

		m_PinkyStateLeft = GetFingerState(
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Pinky_Joint1],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Pinky_Joint2],
			s_JointPositionsLeft[(uint8)EWaveVRHandJoint::Pinky_Tip],
			true
		);
	}
	if (validPoseRight)
	{
		m_ThumbStateRight = GetThumbState(
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Thumb_Joint1],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Thumb_Joint2],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Thumb_Tip],
			false
		);

		m_IndexStateRight = GetFingerState(
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Index_Joint1],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Index_Joint2],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Index_Tip],
			false
		);

		m_MiddleStateRight = GetFingerState(
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Middle_Joint1],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Middle_Joint2],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Middle_Tip],
			false
		);

		m_RingStateRight = GetFingerState(
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Ring_Joint1],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Ring_Joint2],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Ring_Tip],
			false
		);

		m_PinkyStateRight = GetFingerState(
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Pinky_Joint1],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Pinky_Joint2],
			s_JointPositionsRight[(uint8)EWaveVRHandJoint::Pinky_Tip],
			false
		);
	}
}
EWaveVRThumbState AWaveVRCustomGesture::GetThumbState(FVector root, FVector node1, FVector top, bool isLeft)
{
	if (isLeft && !validPoseLeft) { return EWaveVRThumbState::None; }
	if (!isLeft && !validPoseRight) { return EWaveVRThumbState::None; }

	return WaveVRHandHelper::GetThumbState(root, node1, top);
}
EWaveVRFingerState AWaveVRCustomGesture::GetFingerState(FVector root, FVector node1, FVector top, bool isLeft)
{
	if (isLeft && !validPoseLeft) { return EWaveVRFingerState::None; }
	if (!isLeft && !validPoseRight) { return EWaveVRFingerState::None; }

	return WaveVRHandHelper::GetFingerState(root, node1, top);
}
bool AWaveVRCustomGesture::MatchThumbState(FThumbState state)
{
	return false;
}

bool AWaveVRCustomGesture::MatchDistanceSingle(EWaveVRHandJoint node1, EWaveVRHandJoint node2, EWaveVRJointDistance distance, bool isLeft)
{
	if (isLeft && !validPoseLeft) { return false; }
	if (!isLeft && !validPoseRight) { return false; }

	float node_dist = isLeft ?
		FVector::Distance(s_JointPositionsLeft[(uint8)node1], s_JointPositionsLeft[(uint8)node2]) :
		FVector::Distance(s_JointPositionsRight[(uint8)node1], s_JointPositionsRight[(uint8)node2]);

	if (distance == EWaveVRJointDistance::Near) { return (node_dist < 2.5f); }
	return (node_dist > 5);
}
bool AWaveVRCustomGesture::MatchDistanceDual(EWaveVRHandJoint leftNode, EWaveVRHandJoint rightNode, EWaveVRJointDistance distance)
{
	if (!validPoseLeft || !validPoseRight) { return false; }

	float node_dist = FVector::Distance(s_JointPositionsLeft[(uint8)leftNode], s_JointPositionsRight[(uint8)rightNode]);

	if (distance == EWaveVRJointDistance::Near) { return (node_dist < 10); }
	return (node_dist > 20);
}

bool AWaveVRCustomGesture::MatchRotationSingle(FRotator rotateCondition, bool isLeft)
{
	if (isLeft && !validPoseLeft) { return false; }
	if (!isLeft && !validPoseRight) { return false; }


	float Offset = std::ceil(0.2 * 180 / PI);

	float minX = (rotateCondition.Roll > 0) ? (rotateCondition.Roll - Offset) : (rotateCondition.Roll - Offset);
	float maxX = (rotateCondition.Roll > 0) ? (rotateCondition.Roll + Offset) : (rotateCondition.Roll + Offset);

	float minY = rotateCondition.Pitch - Offset;
	float maxY = rotateCondition.Pitch + Offset;

	float minZ = rotateCondition.Yaw - Offset;
	float maxZ = rotateCondition.Yaw + Offset;


	EulerAngles eulerAngles;
	bool xRotate = true, yRotate = true, zRotate = true;
	eulerAngles = isLeft ?
		ToEulerAnglesDegree(s_JointRotationsLeft[(uint8)EWaveVRHandJoint::Palm]) :
		ToEulerAnglesDegree(s_JointRotationsRight[(uint8)EWaveVRHandJoint::Palm]);
	//if (isLeft) {
		//UE_LOG(LogWaveCustomGesture, Log, TEXT("Tick() Left gesture Palm Angle(r,p,y) %f, %f, %f."), eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw);
	//} else
		//UE_LOG(LogWaveCustomGesture, Log, TEXT("Tick() Right gesture Palm Angle(r,p,y) %f, %f, %f."), eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw);
	//UE_LOG(LogWaveCustomGesture, Log, TEXT("Tick() Condition(r,p,y) %f, %f, %f, %f, %f, %f."), minX, maxX, minY, maxY, minZ, maxZ);

	// Skip check when condition valuse is 0
	if (rotateCondition.Roll != 0)
	{
		if (eulerAngles.roll < minX || eulerAngles.roll > maxX) { return false; }
	}
	if (rotateCondition.Pitch != 0)
	{
		if (eulerAngles.pitch < minY || eulerAngles.pitch > maxY) { return false; }
	}
	if (rotateCondition.Yaw != 0)
	{
		if (std::abs(eulerAngles.yaw) < minZ || std::abs(eulerAngles.yaw) > maxZ) { return false; }
	}

	//UE_LOG(LogWaveCustomGesture, Log, TEXT("Tick() MatchRotationSingle result %d %d %d."), xRotate, yRotate, zRotate);
	return (xRotate && yRotate && zRotate);
}

bool AWaveVRCustomGesture::MatchGestureSingle(FSingleHandSetting setting, bool isLeft)
{
	if (isLeft)
	{
		if (!validPoseLeft) { return false; }

		if (!WaveVRHandHelper::MatchThumbState(setting.Thumb, m_ThumbStateLeft)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Index, m_IndexStateLeft)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Middle, m_MiddleStateLeft)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Ring, m_RingStateLeft)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Pinky, m_PinkyStateLeft)) { return false; }
	}
	else
	{
		if (!validPoseRight) { return false; }

		if (!WaveVRHandHelper::MatchThumbState(setting.Thumb, m_ThumbStateRight)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Index, m_IndexStateRight)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Middle, m_MiddleStateRight)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Ring, m_RingStateRight)) { return false; }
		if (!WaveVRHandHelper::MatchFingerState(setting.Pinky, m_PinkyStateRight)) { return false; }
	}

	/// Checks all distance conditions.
	for (int i = 0; i < setting.SingleHandNodeDistances.Num(); i++)
	{
		if (!MatchDistanceSingle(setting.SingleHandNodeDistances[i].Node1, setting.SingleHandNodeDistances[i].Node2, setting.SingleHandNodeDistances[i].Distance, isLeft))
			return false;
	}

	/// Check palm Rotation
	if (!MatchRotationSingle(setting.RotationCondition, isLeft))
		return false;

	return true;
}
bool AWaveVRCustomGesture::MatchGestureDual(FDualHandSetting setting)
{
	if (!validPoseLeft || !validPoseRight) { return false; }

	if (!MatchGestureSingle(setting.LeftHand, true)) { return false; }
	if (!MatchGestureSingle(setting.RightHand, false)) { return false; }

	/// Checks all distance conditions.
	for (int i = 0; i < setting.DualHandNodeDistances.Num(); i++)
	{
		if (!MatchDistanceDual(setting.DualHandNodeDistances[i].LeftNode, setting.DualHandNodeDistances[i].RightNode, setting.DualHandNodeDistances[i].Distance))
			return false;
	}

	return true;
}

EulerAngles AWaveVRCustomGesture::ToEulerAnglesDegree(FQuat q) {
	EulerAngles angles;
	//FRotator angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
	double cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
	angles.roll = FMath::Atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = 2 * (q.W * q.Y - q.Z * q.X);
	if (std::abs(sinp) >= 1)
		angles.pitch = std::copysign(PI / 2, sinp); // use 90 degrees if out of range
	else
		angles.pitch = FMath::Asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
	double cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
	angles.yaw = FMath::Atan2(siny_cosp, cosy_cosp);

	angles.roll = round(angles.roll * 180 / PI);
	angles.pitch = round(angles.pitch * 180 / PI);
	angles.yaw = round(angles.yaw * 180 / PI);
	return angles;
}
