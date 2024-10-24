#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRBlueprintTypes.h"

#include "WaveVRAnchorBlueprintFunctionLibrary.generated.h"


UENUM(BlueprintType, Category = "WaveVR|SpatialAnchor", meta = (DisplayName = "TrackingState"))
enum class EWVRSpatialAnchorTrackingState : uint8 {
	Tracking,
	Paused,
	Stopped
};


USTRUCT(BlueprintType, Category = "WaveVR|SpatialAnchor", meta = (DisplayName = "SpatialAnchorState"))
struct FWVRSpatialAnchorState {
	GENERATED_USTRUCT_BODY()

	FWVRSpatialAnchorState();

	UPROPERTY(BlueprintReadOnly)
	EWVRSpatialAnchorTrackingState trackingState;

	UPROPERTY(BlueprintReadOnly)
	FVector position;

	UPROPERTY(BlueprintReadOnly)
	FQuat rotation;

	UPROPERTY(BlueprintReadOnly)
	FString anchorName;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRSpatialAnchorState& state) {
		uint32 hash = 0;
		hash = HashCombine(hash, GetTypeHash(state.trackingState));
		hash = HashCombine(hash, GetTypeHash(state.position));
		hash = HashCombine(hash, GetTypeHash(state.rotation.Euler()));
		hash = HashCombine(hash, GetTypeHash(state.anchorName));
		return hash;
	}
};

UCLASS()
class WAVEVR_API UWaveVRAnchorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*  Anchor  */
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Creates a Spatial Anchor at the specified position and rotation, with the given anchor name and pose origin model. The anchor is a reference point in the physical space that corresponds to a specific location on a map. It allows for tracking and alignment of virtual content with the real-world environment."))
		static bool CreateSpatialAnchor(const FVector& position, const FRotator& rotation, const FString& anchorName, EWVRPoseOriginModel origin, int64& anchor);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Destroys the Spatial Anchor with the specified anchor ID. When an anchor is destroyed, it is no longer available for tracking and referencing."))
		static bool DestroySpatialAnchor(const int64 anchor);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Retrieves an array of anchor IDs for all existing Spatial Anchors."))
		static bool EnumerateSpatialAnchors(TArray<int64>& anchors);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Retrieves the state of the Spatial Anchor with the specified anchor ID and pose origin model. The state includes information about the anchor's position, rotation, and tracking status."))
		static bool GetSpatialAnchorState(int64 anchor, EWVRPoseOriginModel origin, FWVRSpatialAnchorState& anchorState);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Caches the Spatial Anchor with a unique name. Caching an anchor allows remembering its association with current HMD's map. However if the map disappears, the anchor will also be removed from memory."))
		static bool CacheSpatialAnchor(int64 anchor, const FString& cachedName);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Removes the named Cached Spatial Anchor from the system."))
		static bool UncacheSpatialAnchor(const FString& cachedName);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Retrieves a name array of Cached Spatial Anchors."))
		static bool EnumerateCachedSpatialAnchorNames(TArray<FString>& anchors);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Clears all cached spatial anchors from system."))
		static bool ClearCachedSpatialAnchors();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Creates a Spatial Anchor from a Cached Spatial Anchor. This function creates a new Spatial Anchor with cached properties.  It allows for creating multiple anchors based on a previously cached anchor."))
		static bool CreateSpatialAnchorFromCacheName(const FString& fromCachedName, const FString& ToAnchorName, int64& anchor);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Persists the Spatial anchor by given a unique name. Persisting an anchor ensures that its features and properties, is stored and retained in system. Even if the corresponding map disappears, the persisted anchor remains available."))
		static bool PersistSpatialAnchor(int64 anchor, const FString& pereistedName);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Removes a named Persisted Spatial Anchor."))
		static bool UnpersistSpatialAnchor(const FString& persistedName);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Retrieves a name array of Persisted Spatial Anchors."))
		static bool EnumeratePersistedSpatialAnchorNames(TArray<FString>& anchors);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Clears all Persisted Spatial Anchors."))
		static bool ClearPersistedSpatialAnchors();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Retrieves the maximum and current tracking count for persisted spatial anchors. This allows understanding the limits of how many anchors can be persistently tracked at once."))
		static bool GetPersistedSpatialAnchorCount(int& maximumTrackingCount, int& currentTrackingCount);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Creates a Spatial Anchor from a Persisted Spatial Anchor. This function creates a new Spatial Anchor with remembered properties. It allows for creating multiple anchors based on a previously persisted anchor."))
		static bool CreateSpatialAnchorFromPersistenceName(const FString& fromPersistedName, const FString& ToAnchorName, int64& anchor);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Exports the Persisted Spatial Anchor with the specified name to the file path. This enables saving the data of a persisted anchor to a file for external use or backup purposes."))
		static bool ExportPersistedSpatialAnchor(const FString& persistedSpatialAnchorName, TArray<uint8>& data);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Imports a Persisted Spatial Anchor from the file path. This allows loading a persisted anchor's data from a file into the Spatial Anchor system."))
		static bool ImportPersistedSpatialAnchor(const TArray<uint8>& data);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "In Android, this may return the path of /sdcard/UE4Game/<project name>."))
		static FString GetExternalStorageDir();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Helper function to list all filenames with the extension in the folder."))
		static bool GetFilesInFolder(const FString& dirPath, const FString& extName, TArray<FString>& filenames);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Helper function to save a persisted spatial anchor data to file."))
		static bool SaveData(const TArray<uint8>& data, const FString& filePathname);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SpatialAnchor",
		meta = (ToolTip = "Helper function to load a persisted spatial anchor data from file."))
		static bool LoadData(const FString& filePathname, TArray<uint8>& data);


	/** Returns Hex string of anchor */
	UFUNCTION(BlueprintPure,
		Category = "WaveVR|SpatialAnchor",
		meta = (DisplayName = "ToString (anchor)", BlueprintThreadSafe))
		static FString ToString_Anchor(int64 anchor);

};
