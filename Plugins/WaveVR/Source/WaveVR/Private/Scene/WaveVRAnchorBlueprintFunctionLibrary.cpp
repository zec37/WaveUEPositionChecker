// "WaveVR SDK
// © 2022 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Scene/WaveVRAnchorBlueprintFunctionLibrary.h"
#include "Scene/WaveVRSceneBlueprintFunctionLibrary.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Logging/LogMacros.h"

#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/WaveVRLogWrapper.h"

#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"

#include "WaveVRUtils.h"
#include <Misc/FileHelper.h>

DEFINE_LOG_CATEGORY_STATIC(WVRAnchorBPFL, Log, All);

#define WVR_EMAX(type, value) ((((int32)value) < 0xFF) ? ((type)value) : ((type)0x7FFFFFFF))
#define UE_EMAX(type, value) ((((int32)value) < 0x7FFFFFFF) ? ((type)value) : ((type)0xFF))


using namespace wvr::utils;

namespace wvr {
namespace anchor {
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

	static FString ToUnrealString(const WVR_SpatialAnchorName& anchorNameWVR) {
		return FString(ANSI_TO_TCHAR(anchorNameWVR.name));
	}
}
}

FWVRSpatialAnchorState::FWVRSpatialAnchorState() : trackingState(EWVRSpatialAnchorTrackingState::Stopped), position(FVector::ZeroVector), rotation(FQuat::Identity) {
}

bool UWaveVRAnchorBlueprintFunctionLibrary::CreateSpatialAnchor(const FVector& pos, const FRotator& rot, const FString& anchorName, EWVRPoseOriginModel origin, int64& anchor) {
	WVR_SpatialAnchorCreateInfo createInfo{};

	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;

	createInfo.pose.position = OpenglCoordinate::GetVector3(pos, worldToMeters);
	createInfo.pose.rotation = OpenglCoordinate::GetQuaternion(FQuat(rot));
	createInfo.originModel = (WVR_PoseOriginModel)origin;
	wvr::anchor::ToWVRAnchorName(anchorName, createInfo.anchorName);
	WVR_SpatialAnchor anchorWVR;
	auto ret = WVR()->CreateSpatialAnchor(&createInfo, &anchorWVR);
	anchor = (int64)anchorWVR;
	float& x = createInfo.pose.position.v[0];
	float& y = createInfo.pose.position.v[1];
	float& z = createInfo.pose.position.v[2];
	float& w = createInfo.pose.rotation.w;
	float& i = createInfo.pose.rotation.x;
	float& j = createInfo.pose.rotation.y;
	float& k = createInfo.pose.rotation.z;
	LOGD(WVRAnchorBPFL, "CreateSpatialAnchor %016" PRIX64 " glp=(%3f, %3f, %3f) glq=(w=%3f, %3f, %3f, %3f) o=%d", anchorWVR, x, y, z, w, i, j, k, createInfo.originModel);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("CreateSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::DestroySpatialAnchor(const int64 anchor) {
	LOGD(WVRAnchorBPFL, "DestroySpatialAnchor %016" PRIX64, (uint64_t)anchor);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("DestroySpatialAnchor", WVR()->DestroySpatialAnchor((uint64)anchor));
}

bool UWaveVRAnchorBlueprintFunctionLibrary::EnumerateSpatialAnchors(TArray<int64>& anchors) {
	uint32 anchorCountOutput = 0;
	auto ret = WVR()->EnumerateSpatialAnchors(0, &anchorCountOutput, nullptr);
	LOGD(WVRAnchorBPFL, "EnumerateSpatialAnchors c=%d", anchorCountOutput);
	if (ret != WVR_Success) return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateSpatialAnchors", ret);
	if (anchorCountOutput == 0) return true;

	uint32 anchorCapacityInput = anchorCountOutput;
	auto anchorsWVR = new WVR_SpatialAnchor[anchorCapacityInput];
	ret = WVR()->EnumerateSpatialAnchors(anchorCapacityInput, &anchorCountOutput, anchorsWVR);
	for (uint32 i = 0; i < anchorCountOutput; i++)
		LOGD(WVRAnchorBPFL, "    anchor[%u]=%016" PRIX64, i, anchorsWVR[i]);

	anchors.Empty();
	for (uint32 i = 0; i < anchorCapacityInput; i++)
		anchors.Add((int64)anchorsWVR[i]);

	delete[] anchorsWVR;
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateSpatialAnchors", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::GetSpatialAnchorState(int64 anchor, EWVRPoseOriginModel origin, FWVRSpatialAnchorState& anchorState) {
	WVR_SpatialAnchorState state{};

	auto ret = WVR()->GetSpatialAnchorState((uint64)anchor, (WVR_PoseOriginModel)origin, &state);

	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;

	anchorState.trackingState = (EWVRSpatialAnchorTrackingState)state.trackingState;
	anchorState.position = CoordinateUtil::GetVector3(state.pose.position, worldToMeters);
	anchorState.rotation = CoordinateUtil::GetQuaternion(state.pose.rotation);
	anchorState.rotation.Normalize();
	anchorState.anchorName = ANSI_TO_TCHAR(state.anchorName.name);

	//float& x = state.pose.position.v[0];
	//float& y = state.pose.position.v[1];
	//float& z = state.pose.position.v[2];
	//float& w = state.pose.rotation.w;
	//float& i = state.pose.rotation.x;
	//float& j = state.pose.rotation.y;
	//float& k = state.pose.rotation.z;
	//LOGD(WVRAnchorBPFL, "GetSpatialAnchorState p=(%3f, %3f, %3f) q=(w=%3f, %3f, %3f, %3f) o=%d ts=%d", x, y, z, w, i, j, k, (WVR_PoseOriginModel)origin, (int)state.trackingState);

	// There are issues that the position and rotation are invalid, but the tracking state is Tracking.  To workaround, we check the validity of the transform to avoid the object in Unreal disappear.
	FTransform t = FTransform(anchorState.rotation, anchorState.position, FVector::OneVector);
	if (!t.IsValid()) {
		// This can avoid ensure problem.  In UE427, crash, and In UE53, only error once.
		anchorState.position = FVector::ZeroVector;
		anchorState.rotation = FQuat::Identity;
	}

	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetSpatialAnchorState", ret);
}


bool UWaveVRAnchorBlueprintFunctionLibrary::CacheSpatialAnchor(int64 anchor, const FString& cachedName) {
	WVR_SpatialAnchorCacheInfo createInfo{};
	createInfo.spatialAnchor = anchor;
	wvr::anchor::ToWVRAnchorName(cachedName, createInfo.cachedSpatialAnchorName);
	auto ret = WVR()->CacheSpatialAnchor(&createInfo);
	LOGD(WVRAnchorBPFL, "CacheSpatialAnchor n=%s a=%016" PRIX64, PLATFORM_CHAR(*cachedName), (uint64_t)anchor);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("CacheSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::UncacheSpatialAnchor(const FString& cachedName) {
	WVR_SpatialAnchorName cachedNameWVR{};
	wvr::anchor::ToWVRAnchorName(cachedName, cachedNameWVR);
	auto ret = WVR()->UncacheSpatialAnchor(&cachedNameWVR);
	LOGD(WVRAnchorBPFL, "UncacheSpatialAnchor n=%s", PLATFORM_CHAR(*cachedName));
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("UncacheSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::EnumerateCachedSpatialAnchorNames(TArray<FString>& anchors) {
	uint32_t count = 0;
	auto ret = WVR()->EnumerateCachedSpatialAnchorNames(0, &count, nullptr);
	LOGD(WVRAnchorBPFL, "EnumerateCachedSpatialAnchorNames c=%u", count);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateCachedSpatialAnchorNames", ret)) return false;
	if (count == 0) return true;

	WVR_SpatialAnchorName* cachedSpatialAnchorNames = new WVR_SpatialAnchorName[count];
	ret = WVR()->EnumerateCachedSpatialAnchorNames(count, &count, cachedSpatialAnchorNames);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumerateCachedSpatialAnchorNames", ret)) return false;
	anchors.Empty();
	for (uint32 i = 0; i < count; i++)
		anchors.Add(wvr::anchor::ToUnrealString(cachedSpatialAnchorNames[i]));
	delete[] cachedSpatialAnchorNames;

	return true;
}

bool UWaveVRAnchorBlueprintFunctionLibrary::ClearCachedSpatialAnchors() {
	auto ret = WVR()->ClearCachedSpatialAnchors();
	LOGD(WVRAnchorBPFL, "ClearCachedSpatialAnchors");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ClearCachedSpatialAnchors", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::CreateSpatialAnchorFromCacheName(const FString& fromCachedName, const FString& ToAnchorName, int64& anchor) {
	WVR_SpatialAnchorFromCacheNameCreateInfo info{};
	wvr::anchor::ToWVRAnchorName(fromCachedName, info.cachedSpatialAnchorName);
	wvr::anchor::ToWVRAnchorName(ToAnchorName, info.spatialAnchorName);
	uint64 anchorU = 0;
	auto ret = WVR()->CreateSpatialAnchorFromCacheName(&info, (WVR_SpatialAnchor*)&anchorU);
	anchor = (int64) anchorU;
	LOGD(WVRAnchorBPFL, "CreateSpatialAnchorFromCacheName cn=%s san=%s a=%016" PRIX64,
		PLATFORM_CHAR(*fromCachedName), PLATFORM_CHAR(*ToAnchorName), (uint64_t)anchor);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("CreateSpatialAnchorFromCacheName", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::PersistSpatialAnchor(int64 anchor, const FString& pereistedName) {
	WVR_SpatialAnchorPersistInfo createInfo{};
	createInfo.spatialAnchor = anchor;
	wvr::anchor::ToWVRAnchorName(pereistedName, createInfo.persistedSpatialAnchorName);
	auto ret = WVR()->PersistSpatialAnchor(&createInfo);
	LOGD(WVRAnchorBPFL, "PersistSpatialAnchor  n=%s a=%016" PRIX64, PLATFORM_CHAR(*pereistedName), (uint64_t)anchor);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("PersistSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::UnpersistSpatialAnchor(const FString& persistedName) {
	WVR_SpatialAnchorName persistedNameWVR{};
	wvr::anchor::ToWVRAnchorName(persistedName, persistedNameWVR);
	auto ret = WVR()->UnpersistSpatialAnchor(&persistedNameWVR);
	LOGD(WVRAnchorBPFL, "UnpersistSpatialAnchor n=%s", PLATFORM_CHAR(*persistedName));
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("UnpersistSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::EnumeratePersistedSpatialAnchorNames(TArray<FString>& anchors) {
	uint32_t count = 0;
	auto ret = WVR()->EnumeratePersistedSpatialAnchorNames(0, &count, nullptr);
	LOGD(WVRAnchorBPFL, "EnumeratePersistedSpatialAnchorNames c=%u", count);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumeratePersistedSpatialAnchorNames", ret)) return false;
	if (count == 0) return true;
	WVR_SpatialAnchorName* persistedSpatialAnchorNames = new WVR_SpatialAnchorName[count];
	ret = WVR()->EnumeratePersistedSpatialAnchorNames(count, &count, persistedSpatialAnchorNames);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("EnumeratePersistedSpatialAnchorNames", ret)) return false;
	anchors.Empty();
	for (uint32 i = 0; i < count; i++)
		anchors.Add(wvr::anchor::ToUnrealString(persistedSpatialAnchorNames[i]));
	delete[] persistedSpatialAnchorNames;
	return true;
}

bool UWaveVRAnchorBlueprintFunctionLibrary::ClearPersistedSpatialAnchors() {
	auto ret = WVR()->ClearPersistedSpatialAnchors();
	LOGD(WVRAnchorBPFL, "ClearPersistedSpatialAnchors");
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ClearPersistedSpatialAnchors", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::GetPersistedSpatialAnchorCount(int& maximumTrackingCount, int& currentTrackingCount) {
	WVR_PersistedSpatialAnchorCountGetInfo getInfo{};
	auto ret = WVR()->GetPersistedSpatialAnchorCount(&getInfo);
	maximumTrackingCount = getInfo.maximumTrackingCount;
	currentTrackingCount = getInfo.currentTrackingCount;
	LOGD(WVRAnchorBPFL, "GetPersistedSpatialAnchorCount(%d, %d)", maximumTrackingCount, currentTrackingCount);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("GetPersistedSpatialAnchorCount", ret)) return false;
	return true;
}

bool UWaveVRAnchorBlueprintFunctionLibrary::CreateSpatialAnchorFromPersistenceName(const FString& fromPersistedName, const FString& ToAnchorName, int64& anchor) {
	WVR_SpatialAnchorFromPersistenceNameCreateInfo info{};
	wvr::anchor::ToWVRAnchorName(fromPersistedName, info.persistedSpatialAnchorName);
	wvr::anchor::ToWVRAnchorName(ToAnchorName, info.spatialAnchorName);
	uint64 anchorU = 0;
	auto ret = WVR()->CreateSpatialAnchorFromPersistenceName(&info, (WVR_SpatialAnchor*)&anchorU);
	anchor = (int64) anchorU;
	LOGD(WVRAnchorBPFL, "CreateSpatialAnchorFromPersistenceName cn=%s san=%s a=%016" PRIX64,
		PLATFORM_CHAR(*fromPersistedName), PLATFORM_CHAR(*ToAnchorName), (uint64_t)anchor);
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("CreateSpatialAnchorFromPersistenceName", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::ExportPersistedSpatialAnchor(const FString& targetName, TArray<uint8>& data) {
	WVR_SpatialAnchorName targetNameWVR{};
	wvr::anchor::ToWVRAnchorName(targetName, targetNameWVR);

	uint32_t count = 0;
	auto ret = WVR()->ExportPersistedSpatialAnchor(&targetNameWVR, 0, &count, nullptr);
	LOGD(WVRAnchorBPFL, "ExportPersistedSpatialAnchor c=%u", count);
	if (!UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ExportPersistedSpatialAnchor", ret)) return false;
	if (count == 0) return true;

	data.SetNum(count);
	ret = WVR()->ExportPersistedSpatialAnchor(&targetNameWVR, count, &count, (char*)data.GetData());
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ExportPersistedSpatialAnchor", ret);
}

bool UWaveVRAnchorBlueprintFunctionLibrary::ImportPersistedSpatialAnchor(const TArray<uint8>& data) {

	auto ret = WVR()->ImportPersistedSpatialAnchor(data.Num(), (char*)data.GetData());
	return UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult("ImportPersistedSpatialAnchor", ret);
}

FString UWaveVRAnchorBlueprintFunctionLibrary::GetExternalStorageDir() {
	FString path = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectDir());
	LOGD(WVRAnchorBPFL, "ProjectDir=%s", PLATFORM_CHAR(*path));
#if PLATFORM_ANDROID
	if (true)
#else
	if (false)
#endif
	{
		FString internalPath = FPaths::Combine(TEXT("Android/data"), FApp::GetProjectName());
		LOGD(WVRAnchorBPFL, "internalPath=%s", PLATFORM_CHAR(*internalPath));
		if (path.Contains(internalPath)) {
			path = path.Replace(*internalPath, TEXT(""));
			path = FPaths::Combine(path, FApp::GetProjectName());
			LOGD(WVRAnchorBPFL, "ExternalStorageDir=%s", PLATFORM_CHAR(*path));
		}
	}
	return path;
}

bool UWaveVRAnchorBlueprintFunctionLibrary::GetFilesInFolder(const FString& dirPath, const FString& extName, TArray<FString>& filenames) {
	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(filenames, *dirPath, *extName);
	for (int i = 0; i < filenames.Num(); i++)
		LOGD(WVRAnchorBPFL, "GetFilesInFolder[%d] %s", i, PLATFORM_CHAR(*filenames[i]));
	return true;
}

bool UWaveVRAnchorBlueprintFunctionLibrary::SaveData(const TArray<unsigned char>& data, const FString& filePathname) {
	if (FFileHelper::SaveArrayToFile(data, *filePathname)) {
		LOGD(WVRAnchorBPFL, "ExportPersistedSpatialAnchor save data to %s ", PLATFORM_CHAR(*filePathname));
		return true;
	}
	else {
		LOGE(WVRAnchorBPFL, "ExportPersistedSpatialAnchor unable save data to %s", PLATFORM_CHAR(*filePathname));
		return false;
	}
}

bool UWaveVRAnchorBlueprintFunctionLibrary::LoadData(const FString& filePathname, TArray<unsigned char>& data) {
	if (FFileHelper::LoadFileToArray(data, *filePathname)) {
		LOGD(WVRAnchorBPFL, "ExportPersistedSpatialAnchor load data[%d] from %s ", data.Num(), PLATFORM_CHAR(*filePathname));
		return true;
	}
	else {
		LOGE(WVRAnchorBPFL, "ExportPersistedSpatialAnchor unable load data from %s", PLATFORM_CHAR(*filePathname));
		return false;
	}
}

FString UWaveVRAnchorBlueprintFunctionLibrary::ToString_Anchor(int64 anchor) {
	return FString::Printf(TEXT("%016" PRIX64), (uint64_t)anchor);
}



#undef WVR_EMAX
#undef UE_EMAX
