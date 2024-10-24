#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRBlueprintTypes.h"
#include "Scene/WaveVRSceneBlueprintFunctionLibrary.h"

#include "WaveVRMarkerBlueprintFunctionLibrary.generated.h"


UENUM(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "DetectionTarget"))
enum class EWVRMarkerObserverTarget : uint8 {
	ArUco = 0,
	Max = 0x7F
};

UENUM(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "DetectionState"))
enum class EWVRMarkerObserverState : uint8 {
	Idle = 0,
	Detecting = 1,
	Tracking = 2,
	Max = 0x7F
};


UENUM(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "TrackingState"))
enum class EWVRMarkerTrackingState : uint8 {
	Detected = 0,
	Tracked,
	Paused,
	Stopped
};

USTRUCT(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "ArucoMarker"))
struct FWVRMarkerArUcoMarker {
	GENERATED_USTRUCT_BODY()

	FWVRMarkerArUcoMarker();

	UPROPERTY(BlueprintReadOnly)
	FWVRSceneUuid uuid;

	UPROPERTY(BlueprintReadOnly)
	int64 trackerId;

	UPROPERTY(BlueprintReadOnly)
	float size;

	UPROPERTY(BlueprintReadOnly)
	EWVRMarkerTrackingState state;

	UPROPERTY(BlueprintReadOnly)
	FVector position;

	UPROPERTY(BlueprintReadOnly)
	FQuat rotation;

	UPROPERTY(BlueprintReadOnly)
	FString markerName;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRMarkerArUcoMarker& aruco) {
		uint32 hash = 0;
		hash = HashCombine(hash, GetTypeHash(aruco.uuid));
		hash = HashCombine(hash, GetTypeHash(aruco.trackerId));
		hash = HashCombine(hash, GetTypeHash(aruco.size));
		hash = HashCombine(hash, GetTypeHash(aruco.state));
		hash = HashCombine(hash, GetTypeHash(aruco.position));
		hash = HashCombine(hash, GetTypeHash(aruco.rotation.Euler()));
		hash = HashCombine(hash, GetTypeHash(aruco.markerName));
		return hash;
	}
};

USTRUCT(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "TrackableMarkerState"))
struct FWVRTrackableMarkerState {
	GENERATED_USTRUCT_BODY()

	FWVRTrackableMarkerState();

	UPROPERTY(BlueprintReadOnly)
	EWVRMarkerObserverTarget target;

	UPROPERTY(BlueprintReadOnly)
	EWVRMarkerTrackingState state;

	UPROPERTY(BlueprintReadOnly)
	FVector position;

	UPROPERTY(BlueprintReadOnly)
	FQuat rotation;

	UPROPERTY(BlueprintReadOnly)
	FString markerName;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRTrackableMarkerState& st) {
		uint32 hash = 0;
		hash = HashCombine(hash, GetTypeHash(st.target));
		hash = HashCombine(hash, GetTypeHash(st.state));
		hash = HashCombine(hash, GetTypeHash(st.position));
		hash = HashCombine(hash, GetTypeHash(st.rotation.Euler()));
		hash = HashCombine(hash, GetTypeHash(st.markerName));
		return hash;
	}
};


USTRUCT(BlueprintType, Category = "WaveVR|Marker", meta = (DisplayName = "ArUcoMarkerData"))
struct FWVRArUcoMarkerData {
	GENERATED_USTRUCT_BODY()

	FWVRArUcoMarkerData();

	UPROPERTY(BlueprintReadOnly)
	int64 trackerId;

	UPROPERTY(BlueprintReadOnly)
	float size;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRArUcoMarkerData& data) {
		uint32 hash = 0;
		hash = HashCombine(hash, GetTypeHash(data.trackerId));
		hash = HashCombine(hash, GetTypeHash(data.size));
		return hash;
	}
};


UCLASS()
class WAVEVR_API UWaveVRMarkerBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Start marker system. Should stop it if not to use anymore for saving power."))
		static bool StartMarker();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Stop marker system."))
		static void StopMarker();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Start a marker observer for a kind of target."))
		static bool StartMarkerObserver(EWVRMarkerObserverTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Stop a marker observer for a kind of target."))
		static bool StopMarkerObserver(EWVRMarkerObserverTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Report the state of marker observer."))
		static bool GetMarkerObserverState(EWVRMarkerObserverTarget target, EWVRMarkerObserverState& state);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Let marker observer marker to start detecting new target marker."))
		static bool StartMarkerDetection(EWVRMarkerObserverTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Let marker observer to stop detecting."))
		static bool StopMarkerDetection(EWVRMarkerObserverTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "If you are in a detection, you will receive all detected markers."))
		static bool GetArUcoMarkers(EWVRPoseOriginModel originModel, TArray<FWVRMarkerArUcoMarker>& markers);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "List all trackable markers you created."))
		static bool EnumerateTrackableMarkers(EWVRMarkerObserverTarget target, TArray<FWVRSceneUuid>& markers);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "You can only create trackable markers during the detection."))
		static bool CreateTrackableMarker(const FWVRSceneUuid& uuid, FString name);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Destroy the trackable marker."))
		static bool DestroyTrackableMarker(const FWVRSceneUuid& uuid);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Start to tracking for the trackable marker."))
		static bool StartTrackableMarkerTracking(const FWVRSceneUuid& uuid);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Stop to tracking for the trackable marker."))
		static bool StopTrackableMarkerTracking(const FWVRSceneUuid& uuid);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Get marker's pose and other information by uuid."))
		static bool GetTrackableMarkerState(const FWVRSceneUuid& uuid, EWVRPoseOriginModel originModel, FWVRTrackableMarkerState& state);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "If the marker is an ArUcoMarker, get its ArUcoMarkerData."))
		static bool GetArUcoMarkerData(const FWVRSceneUuid& uuid, FWVRArUcoMarkerData& data);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Marker",
		meta = (ToolTip = "Set trackable markers non-trackable."))
		static bool ClearTrackableMarkers();
};
