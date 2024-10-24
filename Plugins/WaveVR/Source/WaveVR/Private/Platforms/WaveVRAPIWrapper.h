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

#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CString.h"

#include <string>
#include <vector>

#include "wvr.h"
#include "wvr_device.h"
#include "wvr_events.h"
#include "wvr_render.h"
#include "wvr_projection.h"
#include "wvr_types.h"
#include "wvr_system.h"
#include "wvr_overlay.h"
#include "wvr_arena.h"
#include "wvr_camera.h"
#include "wvr_hand.h"
#include "wvr_ctrller_render_model.h"
#include "wvr_eyetracking.h"
#include "wvr_tracker.h"
#include "wvr_lip.h"
#include "wvr_compatibility.h"
#include "wvr_eyeexp.h"
#include "wvr_scene.h"
#include "wvr_anchor.h"
#include "wvr_marker.h"

typedef struct WVR_LayerSetParams {
	WVR_LayerParams_t textures[2];
} WVR_LayerSetParams_t;

typedef enum {
	WVR_AMCStatus_Off = 0,
	WVR_AMCStatus_UMC = 1,
	WVR_AMCStatus_PMC = 2,
} WVR_AMCStatus;

typedef enum {
	ColorGamut_Native = 0,
	ColorGamut_sRGB = 1,
	ColorGamut_DisplayP3 = 2,
} ColorGamut;

typedef enum : uint8_t {
    EDITOR_Unknown = 0,
    UE_424 = 1,
    UE_425 = 2,
    UE_426 = 3,
} AppEditorVersion;

#define WVR() FWaveVRAPIWrapper::GetInstance()

/* internal */
#define WVR_RenderConfig_InverseAlpha ( (uint64_t)1 << 32 )

class WAVEVR_API FWaveVRAPIWrapper {

	friend class FWaveVRHMD;
	friend class FWaveVRPlugin;

public:
	void Tick();

public:
	static inline WVR_Matrix4f_t GetIdentityMatrix4f() {
		WVR_Matrix4f_t m = {};
		m.m[0][0] = 1;
		m.m[1][1] = 1;
		m.m[2][2] = 1;
		m.m[3][3] = 1;
		return m;
	}

	static inline WVR_Matrix4f_t GetTranslationMatrix4f(float x, float y, float z) {
		WVR_Matrix4f_t m = GetIdentityMatrix4f();
		m.m[0][3] = x;
		m.m[1][3] = y;
		m.m[2][3] = z;
		return m;
	}

	static inline WVR_DevicePosePair_t GetBasicPosePair() {
		WVR_DevicePosePair_t pose = {};
		pose.type = WVR_DeviceType_HMD;
		pose.pose = GetBasicPose(WVR_PoseOriginModel_OriginOnHead);
		return pose;
	}

	static inline WVR_PoseState_t GetBasicPose(WVR_PoseOriginModel originModel) {
		WVR_PoseState_t pose = {};
		pose.isValidPose = true;
		pose.poseMatrix = GetIdentityMatrix4f();
		pose.originModel = originModel;
		return pose;
	}

private:
	APlayerController* PlayerController = nullptr;
	void CheckPlayerController();

public:
	virtual uint64_t GetSupportedFeatures() { return 0xFFFFFFFFFFFFFFFFLL; }
	virtual WVR_InitError Init(WVR_AppType type) { return WVR_InitError_NotInitialized; }
	virtual void Quit() { return ; }
	virtual void GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount) { if (PoseCount > 0 && retPose != nullptr) retPose[0] = GetBasicPosePair(); }
	virtual void GetPoseState(WVR_DeviceType type, WVR_PoseOriginModel originModel, uint32_t predictedMilliSec, WVR_PoseState_t *poseState) { if (poseState != nullptr) *poseState = GetBasicPose(originModel); }
	virtual void SetPosePredictEnabled(WVR_DeviceType type, bool enabled_position_predict, bool enabled_rotation_predict) {}
	virtual bool GetInputButtonState(WVR_DeviceType type, WVR_InputId id) { return false; }
	virtual bool GetInputTouchState(WVR_DeviceType type, WVR_InputId id) { return false; }
	virtual WVR_Axis_t GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id) { return WVR_Axis_t(); }
	virtual bool IsDeviceConnected(WVR_DeviceType type) { return false; }
	virtual uint32_t GetParameters(WVR_DeviceType type, const char* param, char* ret, uint32_t bufferSize) { return 0; }
	virtual WVR_NumDoF GetDegreeOfFreedom(WVR_DeviceType type) { return WVR_NumDoF(0); }
	virtual float GetDeviceBatteryPercentage(WVR_DeviceType type)
	{
		if (type == WVR_DeviceType::WVR_DeviceType_HMD) { return 0.2f; }
		if (type == WVR_DeviceType::WVR_DeviceType_Controller_Left) { return 0.3f; }
		if (type == WVR_DeviceType::WVR_DeviceType_Controller_Right) { return 0.4f; }
		return 0;
	}
	virtual bool PollEventQueue(WVR_Event_t* event);
	virtual bool GetRenderProps(WVR_RenderProps_t* props) { return false; }
	virtual bool SetInputRequest(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size) { return false; }
	//virtual bool SetInteractionMode(WVR_InteractionMode mode) { interactionMode = mode; return true; }
	virtual WVR_InteractionMode GetInteractionMode() { return interactionMode; }
	virtual bool SetGazeTriggerType(WVR_GazeTriggerType type) { gazeType = type; return true; }
	virtual WVR_GazeTriggerType GetGazeTriggerType() { return gazeType; }
	virtual bool GetDeviceErrorState(WVR_DeviceType dev_type, WVR_DeviceErrorStatus error_type) { return false; }
	virtual void SetNeckModelEnabled(bool enabled) {}
	virtual WVR_DeviceType GetDefaultControllerRole() { return WVR_DeviceType::WVR_DeviceType_Invalid; }
	virtual void SetArmSticky(bool stickyArm) {}
	virtual void SetArmModel(WVR_SimulationType type) {}
	virtual void InAppRecenter(WVR_RecenterType recenterType) {}
	virtual void SetArenaVisible(WVR_ArenaVisible config) {}
	virtual bool StartCamera(WVR_CameraInfo_t *info) { return false; }
	virtual void StopCamera() {}
	virtual bool GetCameraFrameBuffer(uint8_t *pframebuffer, uint32_t frameBufferSize) { return false; }
	virtual bool RequestScreenshot(uint32_t width, uint32_t height, WVR_ScreenshotMode mode, const char* filename) { return false; }
	virtual uint32_t GetWaveRuntimeVersion() { return 0; }
	virtual bool GetInputMappingPair(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair) { return false; }
	virtual uint32_t GetInputMappingTable(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size) { return 0; }
	virtual bool IsInputFocusCapturedBySystem() { return false; }
	virtual void TriggerVibration(WVR_DeviceType type, WVR_InputId id = WVR_InputId_Max, uint32_t durationMicroSec = 65535, uint32_t frequency = 1, WVR_Intensity intensity = WVR_Intensity_Normal) {}
	virtual void TriggerVibrationScale(WVR_DeviceType type, WVR_InputId inputId = WVR_InputId_Max, uint32_t durationMicroSec = 65535, uint32_t frequency = 1, float amplitude = 0.0f) {}
	virtual bool SetControllerPoseMode(WVR_DeviceType type, WVR_ControllerPoseMode mode) { return false; }
	virtual bool GetControllerPoseMode(WVR_DeviceType type, WVR_ControllerPoseMode *mode) { return false; }
	virtual bool GetControllerPoseModeOffset(WVR_DeviceType type, WVR_ControllerPoseMode mode, WVR_Vector3f_t *translation, WVR_Quatf_t *quaternion) { return false; }
	virtual bool IsDeviceTableStatic(WVR_DeviceType type) { return false; }
	virtual void SetParameters(WVR_DeviceType type, const char *pchValue) {}
#if 0 /* PTW_SUPPORT */
	virtual void SetATWConfig(WVR_DepthOptions opts) {}
#endif
	virtual bool ShowPassthroughOverlay(bool show, bool delaySubmit = false, bool showIndicator = false) { return false; }
	virtual bool IsPassthroughOverlayVisible() { return false; }
	virtual WVR_Result SetPassthroughOverlayAlpha(const float alpha) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result ShowPassthroughUnderlay(bool show) { return WVR_Result::WVR_Error_FeatureNotSupport; }

	virtual WVR_Result ShowProjectedPassthrough(bool show) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result SetProjectedPassthroughAlpha(float alpha) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result SetProjectedPassthroughPose(const WVR_Pose_t* pose) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result SetProjectedPassthroughMesh(float* vertexBuffer, uint32_t vertextCount, uint32_t* indices, uint32_t indexCount) { return WVR_Result::WVR_Error_FeatureNotSupport; }

	/* Gesture and Hand Tracking */
	virtual WVR_Result StartHandGesture(uint64_t demands) { return WVR_Result::WVR_Success; }
	virtual void StopHandGesture() {}
	virtual WVR_Result GetHandGestureInfo(WVR_HandGestureInfo_t* info) {
		return WVR_Result::WVR_Error_FeatureNotSupport;
	}
	virtual WVR_Result GetHandGestureData(WVR_HandGestureData *data) {
		*data = gestureData;
		return WVR_Result::WVR_Success;
	}

	virtual WVR_Result StartHandTracking(WVR_HandTrackerType type) { return WVR_Result::WVR_Success; }
	virtual void StopHandTracking(WVR_HandTrackerType type) {}
	virtual WVR_Result GetHandJointCount(WVR_HandTrackerType type, uint32_t* jointCount);
	virtual WVR_Result GetHandTrackerInfo(WVR_HandTrackerType type, WVR_HandTrackerInfo_t* info);
	virtual WVR_Result GetHandTrackingData(WVR_HandTrackerType trackerType,
		WVR_HandModelType modelType,
		WVR_PoseOriginModel originModel,
		WVR_HandTrackingData* skeleton,
		WVR_HandPoseData_t* pose = nullptr);
	virtual void WearWristTracker(bool wear) { fusionBracelet = wear; }
	virtual bool IsWearingTracker() { return fusionBracelet; }

	/** Tracker **/
	bool m_TrackerEnabled = false;
	virtual WVR_Result StartTracker() {
		m_TrackerEnabled = true;
		return WVR_Result::WVR_Success;
	}
	virtual void StopTracker() { m_TrackerEnabled = false; }
	virtual bool IsTrackerConnected(WVR_TrackerId trackerId)
	{
		if (m_TrackerEnabled) { return true; }
		return false;
	}
	virtual WVR_TrackerRole GetTrackerRole(WVR_TrackerId trackerId)
	{
		if (trackerId == WVR_TrackerId::WVR_TrackerId_0) { return WVR_TrackerRole::WVR_TrackerRole_Pair1_Right; }
		if (trackerId == WVR_TrackerId::WVR_TrackerId_1) { return WVR_TrackerRole::WVR_TrackerRole_Pair1_Left; }

		return WVR_TrackerRole::WVR_TrackerRole_Undefined;
	}
	virtual WVR_Result GetTrackerCapabilities(WVR_TrackerId trackerId, WVR_TrackerCapabilities_t* capabilities)
	{
		capabilities->supportsOrientationTracking = true;
		capabilities->supportsPositionTracking = true;
		capabilities->supportsInputDevice = true;
		capabilities->supportsHapticVibration = true;
		capabilities->supportsBatteryLevel = true;

		return WVR_Result::WVR_Success;
	}
	virtual WVR_Result GetTrackerPoseState(WVR_TrackerId trackerId, WVR_PoseOriginModel originModel, uint32_t predictedMilliSec, WVR_PoseState_t *poseState)
	{
		poseState->isValidPose = true;
		if (trackerId == WVR_TrackerId::WVR_TrackerId_0 || trackerId == WVR_TrackerId::WVR_TrackerId_1)
		{
			poseState->is6DoFPose = true;
		}
		else
		{
			poseState->is6DoFPose = false;
		}
		return WVR_Result::WVR_Success;
	}
	virtual int32_t GetTrackerInputDeviceCapability(WVR_TrackerId trackerId, WVR_InputType inputType)
	{
		if (inputType == WVR_InputType::WVR_InputType_Button)
		{
			return (int32_t)(
				WVR_InputId::WVR_InputId_Alias1_System |
				WVR_InputId::WVR_InputId_Alias1_Menu |
				WVR_InputId::WVR_InputId_Alias1_A |
				WVR_InputId::WVR_InputId_Alias1_B |
				WVR_InputId::WVR_InputId_Alias1_X |
				WVR_InputId::WVR_InputId_Alias1_Y |
				WVR_InputId::WVR_InputId_Alias1_Trigger);
		}
		return 0;
	}
	virtual WVR_AnalogType GetTrackerInputDeviceAnalogType(WVR_TrackerId trackerId, WVR_InputId id)
	{
		if (id == WVR_InputId::WVR_InputId_Alias1_Grip) { return WVR_AnalogType::WVR_AnalogType_1D; }
		if (id == WVR_InputId::WVR_InputId_Alias1_Touchpad) { return WVR_AnalogType::WVR_AnalogType_2D; }
		if (id == WVR_InputId::WVR_InputId_Alias1_Trigger) { return WVR_AnalogType::WVR_AnalogType_1D; }
		if (id == WVR_InputId::WVR_InputId_Alias1_Thumbstick) { return WVR_AnalogType::WVR_AnalogType_2D; }
		
		return WVR_AnalogType::WVR_AnalogType_None;
	}
	virtual bool GetTrackerInputButtonState(WVR_TrackerId trackerId, WVR_InputId id) { return false; }
	virtual bool GetTrackerInputTouchState(WVR_TrackerId trackerId, WVR_InputId id) { return false; }
	virtual WVR_Axis_t GetTrackerInputAnalogAxis(WVR_TrackerId trackerId, WVR_InputId id)
	{
		WVR_Axis_t axis;
		axis.x = 0;
		axis.y = 0;
		return axis;
	}
	virtual float GetTrackerBatteryLevel(WVR_TrackerId trackerId) { return 0.8f; }
	virtual WVR_Result TriggerTrackerVibration(WVR_TrackerId trackerId, uint32_t durationMicroSec = 65535, uint32_t frequency = 0, float amplitude = 0.0f)
	{
		return WVR_Result::WVR_Success;
	}
	static const int32_t kTrackerExtSize = 4;
	int32_t tracker0ExtData[kTrackerExtSize] = { 0, 0, 0, 0 };
	int32_t tracker1ExtData[kTrackerExtSize] = { 1, 1, 1, 1 };
	virtual int32_t* GetTrackerExtendedData(WVR_TrackerId trackerId, int32_t* validSize, uint64_t* timestamp)
	{
		*validSize = kTrackerExtSize;
		if (trackerId == WVR_TrackerId::WVR_TrackerId_0)
			return tracker0ExtData;
		else
			return tracker1ExtData;
		*timestamp = (uint64_t)trackerId;
	}
	const int32_t kTrackerCount = 9;
	static const uint32_t kDeviceNameSizeTrackerUndefined = 10;
	static const uint32_t kDeviceNameSizeTracker0 = 12;
	static const uint32_t kDeviceNameSizeTracker1 = 11;
	const char kDeviceNameTracker0[kDeviceNameSizeTracker0] = "Pair1_Right", kDeviceNameTracker1[kDeviceNameSizeTracker1] = "Pair1_Left", kDeviceNameTrackerUndefined[kDeviceNameSizeTrackerUndefined] = "Undefined";
	virtual WVR_Result GetTrackerDeviceName(WVR_TrackerId trackerId, uint32_t* nameSize, char* deviceName)
	{
		if (deviceName == nullptr)
		{
			if (trackerId == WVR_TrackerId::WVR_TrackerId_0)
			{
				*nameSize = strlen(kDeviceNameTracker0);
				return WVR_Result::WVR_Success;
			}
			if (trackerId == WVR_TrackerId::WVR_TrackerId_1)
			{
				*nameSize = strlen(kDeviceNameTracker1);
				return WVR_Result::WVR_Success;
			}

			*nameSize = strlen(kDeviceNameTrackerUndefined);
			return WVR_Result::WVR_Error_FeatureNotSupport;
		}
		else
		{
			if (trackerId == WVR_TrackerId::WVR_TrackerId_0)
			{
				FCStringAnsi::Strcpy(deviceName, (*nameSize) + 1, kDeviceNameTracker0);
				return WVR_Result::WVR_Success;
			}
			if (trackerId == WVR_TrackerId::WVR_TrackerId_1)
			{
				FCStringAnsi::Strcpy(deviceName, (*nameSize) + 1, kDeviceNameTracker1);
				return WVR_Result::WVR_Success;
			}

			FCStringAnsi::Strcpy(deviceName, (*nameSize) + 1, kDeviceNameTrackerUndefined);
			return WVR_Result::WVR_Error_FeatureNotSupport;
		}
	}
	virtual WVR_Result RegisterTrackerInfoCallback(WVR_TrackerInfoNotify_t* notify) { return WVR_Result::WVR_Success; }
	virtual WVR_Result UnregisterTrackerInfoCallback() { return WVR_Result::WVR_Success; }

	/* Lip Expression */
	bool mLipExpEnabled = false;
	virtual WVR_Result StartLipExp() { mLipExpEnabled = true; return WVR_Result::WVR_Success; }
	virtual void StopLipExp() { mLipExpEnabled = false; }

	static const int32_t kLipExpDataSize = 37; // WVR_LipExpression.WVR_LipExpression_Max
	float lipExpData[kLipExpDataSize] = {
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
		0.1f
	};
	virtual WVR_Result GetLipExpData(float* value)
	{
		if (value == nullptr || !mLipExpEnabled) { return WVR_Result::WVR_Error_FeatureNotSupport; }

		for (int i = 0; i < kLipExpDataSize; i++)
			value[i] = lipExpData[i];

		return WVR_Result::WVR_Success;
	}

	/* Eye */
	virtual WVR_Result StartEyeTracking() { return WVR_Result::WVR_Success; }
	virtual void StopEyeTracking() {}
	virtual WVR_Result GetEyeTracking(WVR_EyeTracking_t *data, WVR_CoordinateSystem space = WVR_CoordinateSystem_Global) {
		*data = eyeData;
		return WVR_Result::WVR_Success;
	}

	/* Eye Expression */
	bool mEyeExpEnabled = false;
	virtual WVR_Result StartEyeExp() { mEyeExpEnabled = true; return WVR_Result::WVR_Success; }
	virtual void StopEyeExp() { mEyeExpEnabled = false; }

	static const int32_t kEyeExpDataSize = 14; // WVR_EyeExpression::WVR_EYEEXPRESSION_MAX
	float eyeExpData[kEyeExpDataSize] = {
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f,
		0.1f, 0.2f, 0.3f, 0.4f, 0.5f
	};
	virtual WVR_Result GetEyeExpData(WVR_EyeExp_t* value)
	{
		if (value == nullptr || !mEyeExpEnabled) { return WVR_Result::WVR_Error_FeatureNotSupport; }

		for (int i = 0; i < kEyeExpDataSize; i++)
			value->weights[i] = eyeExpData[i];

		return WVR_Result::WVR_Success;
	}

private:
	const int kButtonCount = 32;
	uint64_t framePollEventQueue = 0;
	bool AllowPollEventQueue();
	TMap<WVR_DeviceType, TArray<bool>> s_ButtonPressed;
	TMap<WVR_TrackerId, TArray<bool>> s_TrackerPressed;

	WVR_InteractionMode interactionMode = WVR_InteractionMode::WVR_InteractionMode_Controller;
	WVR_GazeTriggerType gazeType = WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout;

	/* Hand */
	bool fusionBracelet = false;

	/* Gesture */
	WVR_HandGestureData_t gestureData;

	/* Eye */
	WVR_EyeTracking_t eyeData;

public:
	virtual WVR_RenderError RenderInit(const WVR_RenderInitParams_t* param) { return WVR_RenderError(0); }
	virtual bool SetColorGamut(ColorGamut gamut) { return false; }
	virtual WVR_Result SetAMCMode(WVR_AMCMode mode) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_AMCMode GetAMCMode() { return WVR_AMCMode::WVR_AMCMode_Off; }
	virtual WVR_AMCStatus GetAMCStatus() { return WVR_AMCStatus::WVR_AMCStatus_Off; }

	virtual WVR_Matrix4f_t GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof = WVR_NumDoF_6DoF) { return GetTranslationMatrix4f(eye == WVR_Eye_Left ? -0.03f : 0.03f, 0.10f, -0.10f); }
	virtual WVR_SubmitError SubmitFrame(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose = NULL, WVR_SubmitExtend extendMethod = WVR_SubmitExtend_Default) { return WVR_SubmitError(0); }
	virtual WVR_TextureQueueHandle_t ObtainTextureQueue(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level) { return WVR_TextureQueueHandle_t(); }
	virtual uint32_t GetTextureQueueLength(WVR_TextureQueueHandle_t handle) { return 1; }
	virtual int32_t GetAvailableTextureIndex(WVR_TextureQueueHandle_t handle) { return 0; }
	virtual WVR_TextureParams_t GetTexture(WVR_TextureQueueHandle_t handle, int32_t index) { return WVR_TextureParams_t(); }
	virtual void ReleaseTextureQueue(WVR_TextureQueueHandle_t handle) {}
	virtual bool IsRenderFoveationSupport() { return false; }
	virtual bool IsRenderFoveationEnabled() { return false; }
	virtual WVR_Result RenderFoveationMode(WVR_FoveationMode mode) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result SetFoveationConfig(const WVR_Eye eye, const WVR_RenderFoveationParams_t *foveatedParam) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetFoveationDefaultConfig(const WVR_Eye eye, WVR_RenderFoveationParams_t *foveatedParam) { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual bool IsAdaptiveQualityEnabled() { return false; }
	virtual bool EnableAdaptiveQuality(bool enable, uint32_t strategyFlags = WVR_QualityStrategy_Default) { return false; }
	virtual WVR_Result EnableTimeWarpStabilizedMode(WVR_TimeWarpStabilizedMode mode) { return WVR_Result::WVR_Error_FeatureNotSupport; };
	virtual void GetRenderTargetSize(uint32_t* width, uint32_t * height) { *width = 1024; *height = 1024; }
	virtual void GetClippingPlaneBoundary(WVR_Eye eye, float * left, float * right, float * top, float * bottom) { *right = *top = 1; *left = *bottom = -1; }
	virtual void PreRenderEye(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam = NULL) {}
	virtual void ReleaseTexture(int32_t id) {}
	virtual WVR_TextureQueueHandle_t StoreRenderTextures(uint32_t texturesIDs[], int size, bool eEye, WVR_TextureTarget target) { return WVR_TextureQueueHandle_t(); }
	virtual WVR_Result SetFrameSharpnessEnhancementLevel(float level) { return WVR_Result::WVR_Error_FeatureNotSupport; }

	/* Overlay */
	virtual WVR_OverlayError GenOverlay(int32_t *overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError DelOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError SetOverlayTextureId(int32_t overlayId, const WVR_OverlayTexture_t *texture) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError SetOverlayFixedPosition(int32_t overlayId, const WVR_OverlayPosition_t *position) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError ShowOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError HideOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual bool IsOverlayValid(int32_t overlayId) { return true; }
	virtual bool IsOverlayShow(int32_t overlayId) { return false; }

	/* Scene */
	virtual WVR_Result StartScene() { return WVR_Success; }
	virtual void StopScene() {}
	virtual WVR_Result StartScenePerception(WVR_ScenePerceptionTarget target); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result StopScenePerception(WVR_ScenePerceptionTarget target); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetScenePerceptionState(WVR_ScenePerceptionTarget target, WVR_ScenePerceptionState* state); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetScenePlanes(const WVR_ScenePlaneFilter* planeFilter, uint32_t planeCapacityInput, uint32_t* planeCountOutput, WVR_PoseOriginModel originModel, WVR_ScenePlane* planes); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetSceneObjects(uint32_t objectCapacityInput, uint32_t* objectCountOutput, WVR_PoseOriginModel originModel, WVR_SceneObject* objects); // has dummy function
	virtual WVR_Result GetSceneMeshes(WVR_SceneMeshType meshType, uint32_t meshCapacityInput, uint32_t* meshCountOutput, WVR_SceneMesh* meshes); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetSceneMeshBuffer(uint64_t meshBufferId, WVR_PoseOriginModel originModel, WVR_SceneMeshBuffer* sceneMeshBuffer); // has dummy function { return WVR_Result::WVR_Error_FeatureNotSupport; }

	/* Anchor */
	virtual WVR_Result CreateSpatialAnchor(const WVR_SpatialAnchorCreateInfo* createInfo, WVR_SpatialAnchor* anchor);
	virtual WVR_Result DestroySpatialAnchor(WVR_SpatialAnchor anchor);
	virtual WVR_Result EnumerateSpatialAnchors(uint32_t anchorCapacityInput, uint32_t* anchorCountOutput, WVR_SpatialAnchor* anchors);
	virtual WVR_Result GetSpatialAnchorState(WVR_SpatialAnchor anchor, WVR_PoseOriginModel origin, WVR_SpatialAnchorState* anchorState);
	virtual WVR_Result CacheSpatialAnchor(const WVR_SpatialAnchorCacheInfo* spatialAnchorCacheInfo);
	virtual WVR_Result UncacheSpatialAnchor(const WVR_SpatialAnchorName* cachedSpatialAnchorName);
	virtual WVR_Result EnumerateCachedSpatialAnchorNames(uint32_t cachedSpatialAnchorNamesCapacityInput, uint32_t* cachedSpatialAnchorNamesCountOutput, WVR_SpatialAnchorName* cachedSpatialAnchorNames);
	virtual WVR_Result ClearCachedSpatialAnchors();
	virtual WVR_Result CreateSpatialAnchorFromCacheName(const WVR_SpatialAnchorFromCacheNameCreateInfo* createInfo, WVR_SpatialAnchor* anchor);
	virtual WVR_Result PersistSpatialAnchor(const WVR_SpatialAnchorPersistInfo* spatialAnchorPersistInfo);
	virtual WVR_Result UnpersistSpatialAnchor(const WVR_SpatialAnchorName* persistedSpatialAnchorName);
	virtual WVR_Result EnumeratePersistedSpatialAnchorNames(uint32_t persistedSpatialAnchorNamesCapacityInput, uint32_t* persistedSpatialAnchorNamesCountOutput, WVR_SpatialAnchorName* persistedSpatialAnchorNames);
	virtual WVR_Result ClearPersistedSpatialAnchors();
	virtual WVR_Result GetPersistedSpatialAnchorCount(WVR_PersistedSpatialAnchorCountGetInfo* getInfo);
	virtual WVR_Result CreateSpatialAnchorFromPersistenceName(const WVR_SpatialAnchorFromPersistenceNameCreateInfo* createInfo, WVR_SpatialAnchor* anchor);
	virtual WVR_Result ExportPersistedSpatialAnchor(const WVR_SpatialAnchorName* persistedSpatialAnchorName, uint32_t dataCapacityInput, uint32_t* dataCountOutput, char* data);
	virtual WVR_Result ImportPersistedSpatialAnchor(uint32_t dataCount, const char* data);

	/* Marker */
	virtual WVR_Result StartMarker();
	virtual void StopMarker();
	virtual WVR_Result StartMarkerObserver(WVR_MarkerObserverTarget target);
	virtual WVR_Result StopMarkerObserver(WVR_MarkerObserverTarget target);
	virtual WVR_Result GetMarkerObserverState( WVR_MarkerObserverTarget target, WVR_MarkerObserverState* state);
	virtual WVR_Result StartMarkerDetection(WVR_MarkerObserverTarget target);
	virtual WVR_Result StopMarkerDetection(WVR_MarkerObserverTarget target);
	virtual WVR_Result GetArucoMarkers(uint32_t markerCapacityInput, uint32_t* markerCountOutput, WVR_PoseOriginModel originModel, WVR_ArucoMarker* markers);
	virtual WVR_Result EnumerateTrackableMarkers(WVR_MarkerObserverTarget target, uint32_t markerCapacityInput, uint32_t* markerCountOutput, WVR_Uuid* uuids);
	virtual WVR_Result CreateTrackableMarker(const WVR_TrackableMarkerCreateInfo* createInfo);
	virtual WVR_Result DestroyTrackableMarker(WVR_Uuid uuid);
	virtual WVR_Result StartTrackableMarkerTracking(WVR_Uuid uuid);
	virtual WVR_Result StopTrackableMarkerTracking(WVR_Uuid uuid);
	virtual WVR_Result GetTrackableMarkerState(WVR_Uuid uuid,WVR_PoseOriginModel originModel, WVR_TrackableMarkerState* state);
	virtual WVR_Result GetArucoMarkerData(WVR_Uuid uuid, WVR_ArucoMarkerData *);
	virtual WVR_Result ClearTrackableMarkers();

	/* WVR internal API */
	virtual bool IsATWActive() { return false; }
	virtual void SetATWActive(bool isActive, void *anativeWindow = nullptr) {}
	virtual void PauseATW() {}  // New Api to replace SetATWActive(false)
	virtual void ResumeATW() {}  // New Api to replace SetATWActive(true)
	virtual void OnDisable() {}  // New Api to replace SetATWActive(false) on Unity3D OnDisable
	virtual void OnApplicationQuit() {}  // New Api to handle OnApplicationQuit case
	virtual void SetRenderThreadId(int tid) {}
	virtual void SetAppEditorVersion(AppEditorVersion v) {}
	virtual void GetStencilMesh(WVR_Eye eye, uint32_t* vertexCount, uint32_t* triangleCount, uint32_t floatArrayCount, float* vertexData, uint32_t intArrayCount, int* indexData) {}
	virtual uint32_t GetAvailableTextureID(WVR_TextureQueueHandle_t handle) { return 1; }
	virtual WVR_TextureQueueHandle_t ReplaceCurrentTextureID(WVR_TextureQueueHandle_t handle, void* texturesIDs) { return WVR_TextureQueueHandle_t(); }

	virtual void SetFocusedController(WVR_DeviceType focusedController) { focusController = focusedController; }
	virtual WVR_DeviceType GetFocusedController() { return focusController; }

	virtual void SetFocusedTracker(int focusedTracker) { focuseTracker = focusedTracker; }
	virtual int GetFocusedTracker() { return focuseTracker; }
private:
	WVR_DeviceType focusController;
	int focuseTracker;

public:
	/* WVR internal API - Resource Wrapper */
	virtual std::string GetStringBySystemLanguage(std::string str) { return std::string(""); }
	virtual std::string GetStringByLanguage(std::string str, std::string lang, std::string country) { return std::string(""); }
	virtual std::string GetSystemLanguage() { return std::string(""); }
	virtual std::string GetSystemCountry() { return std::string(""); }

	/* WVR internal API - OEM Config*/
	virtual std::string GetOEMConfigRawData(std::string key) { return std::string(""); }

	/* WVR internal API - Controller Loader*/
	virtual std::string DeployRenderModelAssets(int deviceIndex, std::string renderModelName) { return std::string(""); }
	virtual std::string GetRootRelativePath() { return std::string(""); }

	/* WVR internal API - Permission Manager */
	virtual bool RequestPermissions(std::vector<std::string> permissions) { return false; }
	virtual bool RequestUsbPermission() { return false; }
	virtual bool IsPermissionGranted(std::string permission) { return false; }
	virtual bool ShowDialogOnVRScene() { return false; }

	/* WVR internal API - Camera Utility*/
	virtual bool CamUtil_GetFrameBufferWithPoseState(uint8_t *pframebuffer, uint32_t frameBufferSize, WVR_PoseOriginModel origin, uint32_t predictInMs, WVR_PoseState_t *poseState) { return false; }
	virtual void CamUtil_ReleaseCameraTexture() {}
	virtual bool CamUtil_DrawTextureWithBuffer(uint32_t textureid, WVR_CameraImageFormat imgFormat, uint8_t *frameBuffer, uint32_t size, uint32_t width, uint32_t height, bool enableCropping, bool clearClampRegion, bool noErrorContext) { return false; }

	/* WVR internal API - Controller model shared from runtime */
	virtual WVR_Result GetCurrentControllerModel(WVR_DeviceType ctrlerType, WVR_CtrlerModel_t **ctrlerModel, bool isOneBone = true) { return WVR_Result::WVR_Error_CtrlerModel_InvalidModel; }
	virtual void ReleaseControllerModel(WVR_CtrlerModel_t **ctrlerModel) {}

	/* WVR internal API - Multi-Layer */
	virtual WVR_SubmitError SubmitCompositionLayers(const WVR_LayerSetParams_t* param) { return WVR_SubmitError_None; }
	virtual uint32_t GetMaxCompositionLayerCount() { return 0; }

	virtual WVR_Result GetCtrlerModelAnimNodeData(WVR_DeviceType ctrlerType, WVR_CtrlerModelAnimData_t **ctrlModelAnimData) { return WVR_Result::WVR_Error_CtrlerModel_NoAnimationData;	}
	virtual void ReleaseCtrlerModelAnimNodeData(WVR_CtrlerModelAnimData_t **ctrlModelAnimData) {}
	virtual bool SetChecker(bool enable) { return false; }

	virtual WVR_Result GetFrameRate(int* frameRate) { *frameRate = 90; return WVR_Success; }
	virtual WVR_Result SetFrameRate(int frameRate) { return WVR_Error_FeatureNotSupport; }
	virtual WVR_Result GetAvailableFrameRates(TArray<int>& frameRates) { frameRates.Empty();  frameRates.Add(90);  frameRates.Add(75);  return WVR_Success; }
	virtual bool SetPassthroughImageQuality(WVR_PassthroughImageQuality quality) { return false; }
	virtual bool SetPassthroughImageFocus(WVR_PassthroughImageFocus focus) { return false; }
	virtual WVR_Result SetPassthroughImageRate(WVR_PassthroughImageRate rate) { return WVR_Error_FeatureNotSupport; }

public:
	virtual bool LoadLibraries() { return false; }
	virtual void UnLoadLibraries() {}

public:
	inline static FWaveVRAPIWrapper * GetInstance()
	{
		return instance;
	}

	virtual ~FWaveVRAPIWrapper();

protected:
	FWaveVRAPIWrapper();

private:
	static void SetInstance(FWaveVRAPIWrapper * instance);

private:
	static FWaveVRAPIWrapper * instance;
};
