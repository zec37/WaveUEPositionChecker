// "WaveVR SDK
// © 2022 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Scene/WaveVRMarkerBlueprintFunctionLibrary.h"
#include "Scene/WaveVRSceneUtils.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Logging/LogMacros.h"

#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/WaveVRLogWrapper.h"
#include "WaveVRUtils.h"

DEFINE_LOG_CATEGORY_STATIC(WVRMarkerBPFL, Log, All);

#define WVR_EMAX(type, value) ((((int32)value) < 0xFF) ? ((type)value) : ((type)0x7FFFFFFF))
#define UE_EMAX(type, value) ((((int32)value) < 0x7FFFFFFF) ? ((type)value) : ((type)0xFF))

using namespace wvr::utils;

FWVRMarkerArUcoMarker::FWVRMarkerArUcoMarker() : trackerId(0), size(0), state(EWVRMarkerTrackingState::Stopped), position(FVector::ZeroVector), rotation(FQuat::Identity) {
}

FWVRTrackableMarkerState::FWVRTrackableMarkerState() : target(EWVRMarkerObserverTarget::Max), state(EWVRMarkerTrackingState::Stopped), position(FVector::ZeroVector), rotation(FQuat::Identity) {}

FWVRArUcoMarkerData::FWVRArUcoMarkerData() : trackerId(0), size(0) {}

bool UWaveVRMarkerBlueprintFunctionLibrary::StartMarker() {
	LOGD(WVRMarkerBPFL, "StartMarker");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StartMarker", WVR()->StartMarker());
}
void UWaveVRMarkerBlueprintFunctionLibrary::StopMarker() {
	LOGD(WVRMarkerBPFL, "StopMarker");
	WVR()->StopMarker();
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StartMarkerObserver(EWVRMarkerObserverTarget target) {
	LOGD(WVRMarkerBPFL, "StartMarkerObserver");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StartMarkerObserver", WVR()->StartMarkerObserver((WVR_MarkerObserverTarget)target));
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StopMarkerObserver(EWVRMarkerObserverTarget target) {
	LOGD(WVRMarkerBPFL, "StopMarkerObserver");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StopMarkerObserver", WVR()->StopMarkerObserver((WVR_MarkerObserverTarget)target));
}
bool UWaveVRMarkerBlueprintFunctionLibrary::GetMarkerObserverState(EWVRMarkerObserverTarget target, EWVRMarkerObserverState& state) {
	WVR_MarkerObserverState stateWVR;
	bool ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetMarkerObserverState", WVR()->GetMarkerObserverState((WVR_MarkerObserverTarget)target, &stateWVR));
	state = ret ? (EWVRMarkerObserverState)stateWVR : EWVRMarkerObserverState::Max;
	return ret;
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StartMarkerDetection(EWVRMarkerObserverTarget target) {
	LOGD(WVRMarkerBPFL, "StartMarkerDetection");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StartMarkerDetection", WVR()->StartMarkerDetection((WVR_MarkerObserverTarget)target));
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StopMarkerDetection(EWVRMarkerObserverTarget target) {
	LOGD(WVRMarkerBPFL, "StopMarkerDetection");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StopMarkerDetection", WVR()->StopMarkerDetection((WVR_MarkerObserverTarget)target));
}

bool UWaveVRMarkerBlueprintFunctionLibrary::GetArUcoMarkers(EWVRPoseOriginModel originModel, TArray<FWVRMarkerArUcoMarker>& markers) {
	markers.Empty();
	uint32_t capacity = 0;
	uint32_t count = 0;
	WVR_ArucoMarker* data = nullptr;
	bool ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetArUcoMarkers1", WVR()->GetArucoMarkers(capacity, &count, (WVR_PoseOriginModel)originModel, data));
	if (!ret || count == 0) return ret;

	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;
	capacity = count;
	data = new WVR_ArucoMarker[count];
	ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetArUcoMarkers2", WVR()->GetArucoMarkers(capacity, &count, (WVR_PoseOriginModel)originModel, data));
	for (uint32_t i = 0; i < capacity; i++) {
		auto& marker = data[i];
		FWVRMarkerArUcoMarker markerUE = {};
		scene::CopyUuid(markerUE.uuid, marker.uuid);
		markerUE.trackerId = (int64)marker.trackerId;
		markerUE.size = marker.size * worldToMeters;
		markerUE.state = (EWVRMarkerTrackingState)marker.state;
		markerUE.position = CoordinateUtil::GetVector3(marker.pose.position, worldToMeters);
		markerUE.rotation = CoordinateUtil::GetQuaternion(marker.pose.rotation) * FQuat(FRotator(0, 180, 0));
		markerUE.rotation.Normalize();
		marker.markerName.name[WVR_MAX_MARKER_NAME_SIZE - 1] = 0;
		markerUE.markerName = marker.markerName.name;
		markers.Add(markerUE);
	}
	delete[] data;
	return ret;
}

bool UWaveVRMarkerBlueprintFunctionLibrary::EnumerateTrackableMarkers(EWVRMarkerObserverTarget target, TArray<FWVRSceneUuid>& markers) {
	markers.Empty();
	uint32_t capacity = 0;
	uint32_t count = 0;
	WVR_Uuid* uuids = nullptr;
	bool ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateTrackableMarkers1", WVR()->EnumerateTrackableMarkers((WVR_MarkerObserverTarget)target, capacity, &count, uuids));
	if (!ret || count == 0) return ret;

	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;
	capacity = count;
	uuids = new WVR_Uuid[count];
	ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateTrackableMarkers2", WVR()->EnumerateTrackableMarkers((WVR_MarkerObserverTarget)target, capacity, &count, uuids));
	for (uint32_t i = 0; i < capacity; i++) {
		auto& uuidWVR = uuids[i];
		FWVRSceneUuid uuid;
		scene::CopyUuid(uuid, uuidWVR);
		markers.Add(uuid);
	}
	delete[] uuids;
	return ret;
}

bool UWaveVRMarkerBlueprintFunctionLibrary::CreateTrackableMarker(const FWVRSceneUuid& uuid, FString name) {
	LOGD(WVRMarkerBPFL, "CreateTrackableMarker(%s, %s)",
		PLATFORM_CHAR(*UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneUuid(uuid)),
		PLATFORM_CHAR(*name));
	WVR_TrackableMarkerCreateInfo createInfo{};

	scene::CopyUuid(createInfo.uuid, uuid);
	std::string str = TCHAR_TO_ANSI(*name);
	int l = str.length();
	size_t m = l < WVR_MAX_MARKER_NAME_SIZE - 1 ? l : WVR_MAX_MARKER_NAME_SIZE - 1;
#if PLATFORM_ANDROID
	strncpy(createInfo.markerName.name, str.c_str(), m);
#else
	strncpy_s(createInfo.markerName.name, str.c_str(), m);
#endif
	createInfo.markerName.name[m] = 0;
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("CreateTrackableMarker", WVR()->CreateTrackableMarker(&createInfo));

}
bool UWaveVRMarkerBlueprintFunctionLibrary::DestroyTrackableMarker(const FWVRSceneUuid& uuid) {
	LOGD(WVRMarkerBPFL, "DestroyTrackableMarker(%s)",
		PLATFORM_CHAR(*UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneUuid(uuid)));
	WVR_Uuid uuidWVR;
	scene::CopyUuid(uuidWVR, uuid);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("DestroyTrackableMarker", WVR()->DestroyTrackableMarker(uuidWVR));
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StartTrackableMarkerTracking(const FWVRSceneUuid& uuid) {
	LOGD(WVRMarkerBPFL, "StartTrackableMarkerTracking(%s)",
		PLATFORM_CHAR (*UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneUuid(uuid)));
	WVR_Uuid uuidWVR;
	scene::CopyUuid(uuidWVR, uuid);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StartTrackableMarkerTracking", WVR()->StartTrackableMarkerTracking(uuidWVR));
}
bool UWaveVRMarkerBlueprintFunctionLibrary::StopTrackableMarkerTracking(const FWVRSceneUuid& uuid) {
	LOGD(WVRMarkerBPFL, "StopTrackableMarkerTracking(%s)",
		PLATFORM_CHAR(*UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneUuid(uuid)));
	WVR_Uuid uuidWVR;
	scene::CopyUuid(uuidWVR, uuid);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("StopTrackableMarkerTracking", WVR()->StopTrackableMarkerTracking(uuidWVR));
}

bool UWaveVRMarkerBlueprintFunctionLibrary::GetTrackableMarkerState(const FWVRSceneUuid& uuid, EWVRPoseOriginModel originModel, FWVRTrackableMarkerState& state) {
	WVR_Uuid uuidWVR;
	scene::CopyUuid(uuidWVR, uuid);
	WVR_TrackableMarkerState stateWVR;
	bool ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetTrackableMarkerState", WVR()->GetTrackableMarkerState(uuidWVR, (WVR_PoseOriginModel)originModel, &stateWVR));
	if (ret) {
		state.target = (EWVRMarkerObserverTarget)stateWVR.target;
		state.state = (EWVRMarkerTrackingState)stateWVR.state;
		stateWVR.markerName.name[WVR_MAX_MARKER_NAME_SIZE - 1] = 0;
		state.markerName = stateWVR.markerName.name;
		state.markerName = state.markerName.TrimEnd();
		state.position = CoordinateUtil::GetVector3(stateWVR.pose.position, 100);
		state.rotation = CoordinateUtil::GetQuaternion(stateWVR.pose.rotation) * FQuat(FRotator(0, 180, 0));
	} else {
		state.target = EWVRMarkerObserverTarget::Max;
		state.state = EWVRMarkerTrackingState::Stopped;
	}
	return ret;
}

bool UWaveVRMarkerBlueprintFunctionLibrary::GetArUcoMarkerData(const FWVRSceneUuid& uuid, FWVRArUcoMarkerData& data)
{
	WVR_Uuid uuidWVR;
	scene::CopyUuid(uuidWVR, uuid);
	WVR_ArucoMarkerData dataWVR = {};
	bool ret = UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetArUcoMarkerData", WVR()->GetArucoMarkerData(uuidWVR, &dataWVR));

	if (ret) {
		data.trackerId = dataWVR.trackerId;
		data.size = dataWVR.size * 100;
	}
	return ret;
}

bool UWaveVRMarkerBlueprintFunctionLibrary::ClearTrackableMarkers() {
	LOGD(WVRMarkerBPFL, "ClearTrackableMarkers");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ClearTrackableMarkers", WVR()->ClearTrackableMarkers());
}

#undef WVR_EMAX
#undef UE_EMAX
