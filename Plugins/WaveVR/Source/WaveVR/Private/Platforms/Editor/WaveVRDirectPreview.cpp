// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Platforms/Editor/WaveVRDirectPreview.h"

#include <chrono>

// Engine
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Platforms/WaveVRLogWrapper.h"
#include "Platforms/DLLFunctionPointer.h"

#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif

// Wave
#include "WaveVRHMD.h"
#if WITH_EDITOR
#include "WaveVRDirectPreviewSettings.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(WVRDirectPreview, Log, All);

#if 0  // Debug only
#undef LOG_FUNC
#define LOG_FUNC() UE_LOG(WVRDirectPreview, Log, TEXT("%s"), WVR_FUNCTION_STRING)
#else
#endif

#define FUNC_CHECK() \
	if (funcPtr == nullptr) \
		UE_LOG(WVRDirectPreview, Error, TEXT("Failed to load Simulator library."));

#define LOG_DP(level, Format) UE_LOG(WVRDirectPreview, level, TEXT(Format));

WaveVRDirectPreview::WaveVRDirectPreview() :
	DllLoader(FString("wvr_plugins_directpreview.dll")) {
}

WaveVRDirectPreview::~WaveVRDirectPreview() {
	mDllHandle = nullptr;
}

bool WaveVRDirectPreview::LoadLibraries() {
	LOG_FUNC();
	bool ret = DllLoader.LoadExternalDLL();
	if (mDllHandle == nullptr && DllLoader.GetHandle() != nullptr) {
		mDllHandle = DllLoader.GetHandle();
	}
	return ret;
}

void WaveVRDirectPreview::UnLoadLibraries() {
	LOG_FUNC();
	DllLoader.UnloadExternalDLL();
}

bool WaveVRDirectPreview::IsDirectPreview() {
	//LOG_FUNC();  // Too noisy
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	return HMD ? HMD->IsDirectPreview() : false;
}

bool WaveVRDirectPreview::IsVRPreview()
{
	//LOG_FUNC();  // Too noisy
#if WITH_EDITOR
	ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
	return PlaySettings->LastExecutedPlayModeType == PlayMode_InVR;
#else
	return false;
#endif
}

bool WaveVRDirectPreview::HookVRPreview() {
	LOG_FUNC();
	if (!GIsEditor)
		return false;

	if (!IsVRPreview())
		return false;

	if (!isInitialized) {
#if WITH_EDITOR
		SetPrintCallback(&DllLog);
		DP_InitError error = DP_InitError_None;
		error = ReadSettingsAndInit();

		if (error == DP_InitError_None)
		{
			isInitialized = true;
			LOG_DP(Log, "InVR enabled");
		}
		else
		{
			ReportSimError(error);
		}
#endif
	}
	return isInitialized;
}

#if 0
		switch (PlaySettings->LastExecutedPlayModeType)
		{
		case PlayMode_InViewPort:
			LOG_DP("InViewPort");
			break;

		case PlayMode_InEditorFloating:
			LOG_DP("InEditorFloating");
			break;

		case PlayMode_InMobilePreview:
			LOG_DP("InMobilePreview");
			break;

		case PlayMode_InTargetedMobilePreview:
			LOG_DP("InTargetedMobilePreview");
			break;

		case PlayMode_InVulkanPreview:
			LOG_DP("InVulkanPreview");
			break;

		case PlayMode_InNewProcess:
			LOG_DP("InNewProcess");
			break;

		case PlayMode_InVR:
			LOG_DP("InVR");
			bIsVRPreview = true;
			break;

		case PlayMode_Simulate:
			LOG_DP("Simulate");
			break;

		default:
			LOG_DP("<UNKNOWN>");
		}
#endif

#if WITH_EDITOR
void WaveVRDirectPreview::ReportSimError(DP_InitError error)
{
	LOG_FUNC();
	switch (error)
	{
	case DP_InitError_None:
		LOG_DP(Log, "WaveVR: DP_InitError_None");
		break;
	case DP_InitError_WSAStartUp_Failed:
		LOG_DP(Error, "WaveVR: DP_InitError_WSAStartUp_Failed");
		break;
	case DP_InitError_Already_Inited:
		LOG_DP(Error, "WaveVR: DP_InitError_Already_Inited");
		break;
	case DP_InitError_Device_Not_Found:
		LOG_DP(Error, "WaveVR: DP_InitError_Device_Not_Found");
		break;
	case DP_InitError_Can_Not_Connect_Server:
		LOG_DP(Error, "WaveVR: DP_InitError_Can_Not_Connect_Server");
		break;
	case DP_InitError_IPAddress_Null:
		LOG_DP(Error, "WaveVR: DP_InitError_IPAddress_Null");
		break;
	default:
		//LOG_DP(Error, "WaveVR: InitError default case");
		break;
	}
}

uintptr_t latestNativeResource = 0;
bool WaveVRDirectPreview::sendRTTexture(FRHICommandListImmediate& RHICmdList, void* NativeResource) {
	LOG_FUNC();
	//UE_LOG(WVRDirectPreview, Display, TEXT("OnBeginRendering_RenderThread sendRTTexture() begin"));

	if (isInitialized) {
        //UE_LOG(WVRDirectPreview, Display, TEXT("OnBeginRendering_RenderThread isInitialized()"));
		if (isTimeToUpdate()) {
			void* rtPtr;
			rtPtr = NativeResource;
			//UE_LOG(WVRDirectPreview, Display, TEXT("OnBeginRendering_RenderThread isTimeToUpdate()"));
			if ((uintptr_t)NativeResource != latestNativeResource) {
				UE_LOG(WVRDirectPreview, Display, TEXT(" Pointer has been changed, \"%" PRIxPTR "\""), NativeResource);
				latestNativeResource = (uintptr_t)NativeResource;
			}
			static _WVR_SetRenderImageHandle funcPtr = nullptr;
			if (mDllHandle != nullptr && funcPtr == nullptr)
			{
				FString procName = "WVR_SetRenderImageHandle";
				funcPtr = (_WVR_SetRenderImageHandle)FPlatformProcess::GetDllExport(mDllHandle, *procName);
				//UE_LOG(WVRDirectPreview, Display, TEXT("sendRTTexture() RR2 WVR_SetRenderImageHandle"));
			}
			FUNC_CHECK();
			FUNC_RUN_1(rtPtr);
		}
	}
    //UE_LOG(WVRDirectPreview, Display, TEXT("OnBeginRendering_RenderThread sendRTTexture() end"));
	return false;
}

void WaveVRDirectPreview::ExportTexture(FRHICommandListImmediate& RHICmdList, FRHITexture2D* TexRef2D)
{
	check(IsInRenderingThread());
	if (TexRef2D == nullptr)
	{
		return;
	}

	//	FTextureReferenceRHIRef MyTextureRHI = Texture->TextureReference.TextureReferenceRHI;
	//	FRHITexture* TexRef = MyTextureRHI->GetTextureReference()->GetReferencedTexture();
	TArray<FColor> Bitmap;
	uint32 LolStride = 0;
	char* TextureDataPtr = (char*)RHICmdList.LockTexture2D(TexRef2D, 0, EResourceLockMode::RLM_ReadOnly, LolStride, false);

	for (uint32 Row = 0; Row < TexRef2D->GetSizeY(); ++Row)
	{
		uint32* PixelPtr = (uint32*)TextureDataPtr;
		for (uint32 Col = 0; Col < TexRef2D->GetSizeX(); ++Col)
		{
			uint32 EncodedPixel = *PixelPtr;
			uint8 r = (EncodedPixel & 0x000000FF);
			uint8 g = (EncodedPixel & 0x0000FF00) >> 10;
			uint8 b = (EncodedPixel & 0x00FF0000) >> 20;
			uint8 a = (EncodedPixel & 0xFF000000) >> 30;
			FColor col = FColor(r, g, b, a);
			Bitmap.Add(FColor(b, g, r, a));
			PixelPtr++;
		}
		// move to next row:
		TextureDataPtr += LolStride;
	}
	RHICmdList.UnlockTexture2D(TexRef2D, 0, false);

	if (Bitmap.Num())
	{
		IFileManager::Get().MakeDirectory(*FPaths::ScreenShotDir(), true);
		TCHAR* ScreenFileName = TEXT("D:/UnrealExport.bmp");
		uint32 ExtendXWithMSAA = Bitmap.Num() / TexRef2D->GetSizeY();
		// Save the contents of the array to a bitmap file. (24bit only so alpha channel is dropped)
		FFileHelper::CreateBitmap(ScreenFileName, ExtendXWithMSAA, TexRef2D->GetSizeY(), Bitmap.GetData());
		UE_LOG(WVRDirectPreview, Display, TEXT("Content was saved to \"%s\""), ScreenFileName);
	}
	else
	{
		UE_LOG(WVRDirectPreview, Error, TEXT("Failed to save BMP, format or texture type is not supported"));
	}
}

DP_InitError WaveVRDirectPreview::SimulatorInit(DP_ConnectType type, const char* IP, bool enablePreview, bool dllToFile, bool saveImage)
{
	LOG_FUNC();

	FUNC_PTR(Init);
	FUNC_RUN_5(type, IP, enablePreview, dllToFile, saveImage);
	FUNC_CHECK();
	return DP_InitError_WSAStartUp_Failed;
}
#endif

void WaveVRDirectPreview::Quit()
{
	LOG_FUNC();
	isInitialized = false;
	PARENT(Quit);
	FUNC_PTR(Quit);
	FUNC_RUN();
	FUNC_CHECK();
}

void WaveVRDirectPreview::GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount)
{
	LOG_FUNC();
	PARENT_3(GetSyncPose, originModel, retPose, PoseCount);
	FUNC_PTR(GetSyncPose);
	FUNC_RUN_3(originModel, retPose, PoseCount);
	FUNC_CHECK();
}

bool WaveVRDirectPreview::GetInputButtonState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	PARENT_2(GetInputButtonState, type, id);
	FUNC_PTR(GetInputButtonState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false; //failure
}

bool WaveVRDirectPreview::GetInputTouchState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	PARENT_2(GetInputTouchState, type, id);
	FUNC_PTR(GetInputTouchState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false;
}

WVR_Axis_t WaveVRDirectPreview::GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	PARENT_2(GetInputAnalogAxis, type, id);
	FUNC_PTR(GetInputAnalogAxis);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	WVR_Axis_t ret;
	ret.x=ret.y=0.0f;
	return ret; //failure
}

bool WaveVRDirectPreview::IsDeviceConnected(WVR_DeviceType type)
{
	LOG_FUNC();
	PARENT_1(IsDeviceConnected, type);
	FUNC_PTR(IsDeviceConnected);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return false; //failure
}

uint32_t WaveVRDirectPreview::GetParameters(WVR_DeviceType type, const char* Parameter, char* retValue, uint32_t bufferSize)
{
	LOG_FUNC();
	PARENT_4(GetParameters, type, Parameter, retValue, bufferSize);
	FUNC_PTR(GetParameters);
	FUNC_RUN_4(type, Parameter, retValue, bufferSize);
	FUNC_CHECK();
	return 0; //failure
}

WVR_NumDoF WaveVRDirectPreview::GetDegreeOfFreedom(WVR_DeviceType type)
{
	if (!isInitialized && IsVRPreview()) {
		LOG_DP(Warning, "Not init, GetDegreeOfFreedom return 3Dof.");
		return WVR_NumDoF_3DoF;
	}
	else {
		LOG_FUNC();
		PARENT_1(GetDegreeOfFreedom, type);
		FUNC_PTR(GetDegreeOfFreedom);
		FUNC_RUN_1(type);
		FUNC_CHECK();
		return WVR_NumDoF_3DoF; //failure
	}
}

bool WaveVRDirectPreview::SetControllerPoseMode(WVR_DeviceType type, WVR_ControllerPoseMode mode)
{
	LOG_FUNC();
	PARENT_2(SetControllerPoseMode, type, mode);
	FUNC_PTR(SetControllerPoseMode);
	FUNC_RUN_2(type, mode);
	FUNC_CHECK();
	return false;
}

bool WaveVRDirectPreview::GetControllerPoseMode(WVR_DeviceType type, WVR_ControllerPoseMode *mode)
{
	LOG_FUNC();
	PARENT_2(GetControllerPoseMode, type, mode);
	FUNC_PTR(GetControllerPoseMode);
	FUNC_RUN_2(type, mode);
	FUNC_CHECK();
	return false;
}

bool WaveVRDirectPreview::GetControllerPoseModeOffset(WVR_DeviceType type, WVR_ControllerPoseMode mode, WVR_Vector3f_t *translation, WVR_Quatf_t *quaternion)
{
	LOG_FUNC();
	PARENT_4(GetControllerPoseModeOffset, type, mode, translation, quaternion);
	FUNC_PTR(GetControllerPoseModeOffset);
	FUNC_RUN_4(type, mode, translation, quaternion);
	FUNC_CHECK();
	return false;
}

float WaveVRDirectPreview::GetDeviceBatteryPercentage(WVR_DeviceType WVR_DeviceType)
{
	LOG_FUNC();
	PARENT_1(GetDeviceBatteryPercentage, WVR_DeviceType);
	FUNC_PTR(GetDeviceBatteryPercentage);
	FUNC_RUN_1(WVR_DeviceType);
	FUNC_CHECK();
	return 0; //failure
}

void WaveVRDirectPreview::GetRenderTargetSize(uint32_t* width, uint32_t* height)
{
	if (!isInitialized && IsVRPreview()) {
		LOG_DP(Warning, "Not init, set default GetRenderTargetSize");
		*width = 1024;
		*height = 1024;
	}
	else {
		LOG_FUNC();
		PARENT_2(GetRenderTargetSize, width, height);
		FUNC_PTR(GetRenderTargetSize);
		FUNC_RUN_2(width, height);
		FUNC_CHECK();
	}
}

void WaveVRDirectPreview::GetClippingPlaneBoundary(WVR_Eye eye, float* left, float* right, float* top, float* bottom)
{
	if (!isInitialized && IsVRPreview()) {
		LOG_DP(Warning, "Not init, set default GetClippingPlaneBoundary");
		*left = -1;
		*right = 1;
		*top = 1;
		*bottom = -1;
	}
	else {
		LOG_FUNC();
		PARENT_5(GetClippingPlaneBoundary, eye, left, right, top, bottom);
		FUNC_PTR(GetClippingPlaneBoundary);
		FUNC_RUN_5(eye, left, right, top, bottom);
		FUNC_CHECK();
	}
}


WVR_Matrix4f_t WaveVRDirectPreview::GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof)
{
	if (!isInitialized) {
		return GetIdentityMatrix4f();
	}
	else {
		LOG_FUNC();
		PARENT_2(GetTransformFromEyeToHead, eye, dof);
		FUNC_PTR(GetTransformFromEyeToHead);
		FUNC_RUN_2(eye, dof);
		FUNC_CHECK();
		return GetIdentityMatrix4f(); // failure
	}
}

bool WaveVRDirectPreview::GetRenderProps(WVR_RenderProps_t* props)
{
	if (!isInitialized && IsVRPreview()) {
		LOG_DP(Warning, "Not init, GetRenderProps return false");
		return false;
	}
	else {
		LOG_FUNC();
		PARENT_1(GetRenderProps, props);
		FUNC_PTR(GetRenderProps);
		FUNC_RUN_1(props);
		FUNC_CHECK();
		return false; // failure
	}
}

bool WaveVRDirectPreview::PollEventQueue(WVR_Event_t* event)
{
	// LOG_FUNC();  // too noisy
	PARENT_1(PollEventQueue, event);
	FUNC_PTR(PollEventQueue);
	FUNC_RUN_1(event);
	FUNC_CHECK();
	return false; //failure
}

WVR_Result WaveVRDirectPreview::GetCurrentControllerModel(WVR_DeviceType ctrlerType, WVR_CtrlerModel_t **ctrlerModel, bool isOneBone)
{
	LOG_FUNC();
	PARENT_3(GetCurrentControllerModel, ctrlerType, ctrlerModel, isOneBone);
	FUNC_PTR(GetCurrentControllerModel);
	FUNC_RUN_3(ctrlerType, ctrlerModel, isOneBone);
	FUNC_CHECK();
	return WVR_Error_CtrlerModel_Unknown;
}

void WaveVRDirectPreview::ReleaseControllerModel(WVR_CtrlerModel_t **ctrlerModel)
{
	LOG_FUNC();
	PARENT_1(ReleaseControllerModel, ctrlerModel);
	FUNC_PTR(ReleaseControllerModel);
	FUNC_RUN_1(ctrlerModel);
	FUNC_CHECK();
}

WVR_InteractionMode WaveVRDirectPreview::GetInteractionMode() {
	LOG_FUNC();
	PARENT(GetInteractionMode);
	static _WVR_GetInteractionMode funcPtr = nullptr;
	WVR_InteractionMode interAct = WVR_InteractionMode::WVR_InteractionMode_SystemDefault;
	FString procName = "WVR_GetInteractionMode_S";	// Needs to be the exact name of the DLL method.
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		funcPtr = (_WVR_GetInteractionMode)FPlatformProcess::GetDllExport(mDllHandle, *procName);
		//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetInteractionMode *s"), funcPtr());
	}
    FUNC_RUN();
    FUNC_CHECK();
    return WVR_InteractionMode::WVR_InteractionMode_SystemDefault;
}

/* Hand */
WVR_Result WaveVRDirectPreview::StartHandGesture(uint64_t demands) {
	LOG_FUNC();
	PARENT_1(StartHandGesture, demands);
	//FUNC_PTR(StartHandGesture);
	static _WVR_StartHandGesture funcPtr = nullptr;
	FString procName = "WVR_StartHandGesture_S";	// Needs to be the exact name of the DLL method.
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		funcPtr = (_WVR_StartHandGesture)FPlatformProcess::GetDllExport(mDllHandle, *procName);
		//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) StartHandGesture "));
	}
    FUNC_RUN_1(demands);
    FUNC_CHECK();
    return WVR_Error_FeatureNotSupport; //Failure
}

void WaveVRDirectPreview::StopHandGesture() {
	LOG_FUNC();
	PARENT(StopHandGesture);
	FUNC_PTR(StopHandGesture);
	FUNC_RUN();
	FUNC_CHECK();
}

WVR_Result WaveVRDirectPreview::GetHandGestureInfo(WVR_HandGestureInfo_t* info) {
	LOG_FUNC();
	PARENT_1(GetHandGestureInfo, info);
	//FUNC_PTR(GetHandGestureInfo);
	static _WVR_GetHandGestureInfo funcPtr = nullptr;
	FString procName = "WVR_GetHandGestureInfo_S";	// Needs to be the exact name of the DLL method.
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		funcPtr = (_WVR_GetHandGestureInfo)FPlatformProcess::GetDllExport(mDllHandle, *procName);
		//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetHandGestureInfo"));
	}
	FUNC_RUN_1(info);
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}

WVR_Result WaveVRDirectPreview::GetHandGestureData(WVR_HandGestureData *data) {
	LOG_FUNC();
	PARENT_1(GetHandGestureData, data);
	//FUNC_PTR(GetHandGestureData);
	static _WVR_GetHandGestureData funcPtr = nullptr;
	FString procName = "WVR_GetHandGestureData_S";	// Needs to be the exact name of the DLL method.
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		funcPtr = (_WVR_GetHandGestureData)FPlatformProcess::GetDllExport(mDllHandle, *procName);
		//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetHandGestureData "));
	}

	FUNC_RUN_1(data);
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}

WVR_Result WaveVRDirectPreview::StartHandTracking(WVR_HandTrackerType type) {
	LOG_FUNC();
	PARENT_1(StartHandTracking, type);
	FUNC_PTR(StartHandTracking);
	//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) StartHandTracking "));
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}
void WaveVRDirectPreview::StopHandTracking(WVR_HandTrackerType type) {
	LOG_FUNC();
	PARENT_1(StopHandTracking, type);
	FUNC_PTR(StopHandTracking);
	//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) StopHandTracking "));
	FUNC_RUN_1(type);
	FUNC_CHECK();
}

WVR_Result WaveVRDirectPreview::GetHandJointCount(WVR_HandTrackerType type, uint32_t* jointCount) {
	LOG_FUNC();
	PARENT_2(GetHandJointCount, type, jointCount);
	FUNC_PTR(GetHandJointCount);
	//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetHandJointCount "));
	FUNC_RUN_2(type, jointCount);
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}
WVR_Result WaveVRDirectPreview::GetHandTrackerInfo(WVR_HandTrackerType type, WVR_HandTrackerInfo_t* info) {
	LOG_FUNC();
	PARENT_2(GetHandTrackerInfo, type, info);
	FUNC_PTR(GetHandTrackerInfo);
	//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetHandTrackerInfo "));
	FUNC_RUN_2(type, info);
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}
WVR_Result WaveVRDirectPreview::GetHandTrackingData(WVR_HandTrackerType trackerType, WVR_HandModelType modelType, WVR_PoseOriginModel originModel, WVR_HandTrackingData* skeleton, WVR_HandPoseData_t* pose) {
	//pose = nullptr;
	LOG_FUNC();
	PARENT_5(GetHandTrackingData, trackerType, modelType, originModel, skeleton, pose);
	FUNC_PTR(GetHandTrackingData);
	//UE_LOG(WVRDirectPreview, Display, TEXT("WaveVRDirectPreview(Editor) GetHandTrackingData "));
	//FUNC_RUN_5(trackerType, modelType, originModel, skeleton, pose);
	//TODO: Workaround to prevent from modify WaveVR_DirectPreview.h. Revert this part once the client app and DLL file have upgraded.
	if (funcPtr != nullptr) {
		WVR_DP_HandTrackingData* handData = new WVR_DP_HandTrackingData;
		handData->right.joints = new WVR_Pose_t[26];
		handData->left.joints = new WVR_Pose_t[26];

		WVR_Result result = funcPtr(trackerType, modelType, originModel, handData, pose);

		skeleton->timestamp = handData->timestamp;
		skeleton->right.isValidPose = handData->right.isValidPose;
		skeleton->left.isValidPose = handData->left.isValidPose;
		skeleton->right.confidence = handData->right.confidence;
		skeleton->left.confidence = handData->left.confidence;
		skeleton->right.jointCount = handData->right.jointCount;
		skeleton->left.jointCount = handData->left.jointCount;
		for (size_t i = 0; i < 26; i++) {
			skeleton->right.joints[i] = handData->right.joints[i];
			skeleton->left.joints[i] = handData->left.joints[i];
		}
		skeleton->right.scale = handData->right.scale;
		skeleton->left.scale = handData->left.scale;
		skeleton->right.wristLinearVelocity = handData->right.wristLinearVelocity;
		skeleton->left.wristLinearVelocity = handData->left.wristLinearVelocity;
		skeleton->right.wristAngularVelocity = handData->right.wristAngularVelocity;
		skeleton->left.wristAngularVelocity = handData->left.wristAngularVelocity;
		delete[] handData->right.joints;
		delete[] handData->left.joints;
		delete handData;
		return result;
	}
	FUNC_CHECK();
	return WVR_Error_FeatureNotSupport; //Failure
}



#if WITH_EDITOR
void WaveVRDirectPreview::SetPrintCallback(PrintLog callback){
	LOG_FUNC();
	static _WVR_SetPrintCallback funcPtr = nullptr;
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		FString procName = "WVR_SetPrintCallback";
		funcPtr = (_WVR_SetPrintCallback) FPlatformProcess::GetDllExport(mDllHandle, *procName); \
	}
	FUNC_CHECK();
	FUNC_RUN_1(callback);
}

void WaveVRDirectPreview::DllLog(const char* msg)
{
	FString str(msg);
	LOGI(WVRDirectPreview, "%s", *str);
}

using namespace std::chrono;
milliseconds latestUpdateTime;
bool WaveVRDirectPreview::isTimeToUpdate() {
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	if ((ms -latestUpdateTime) >= milliseconds(1000 / mFPS)) {
		latestUpdateTime = ms;
		return true;
	}
	else {
		return false;
	}
}

DP_InitError WaveVRDirectPreview::ReadSettingsAndInit() {
	LOG_DP(Warning, "readSettingsAndInit Enter." );
	DP_InitError error = DP_InitError_None;

	FString DeviceWiFiIP = FString(TEXT(""));
	char* mWiFiIP = TCHAR_TO_ANSI(*DeviceWiFiIP);

	error = SimulatorInit((DP_ConnectType)DP_ConnectType_USB, mWiFiIP, true, false, true);
	UE_LOG(WVRDirectPreview, Display, TEXT("ReadSettingsAndInit(Editor) SimulatorInit() %d"), (uint32_t)error);
	return error;
}
#endif
