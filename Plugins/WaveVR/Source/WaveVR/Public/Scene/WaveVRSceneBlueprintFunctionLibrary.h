#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRBlueprintTypes.h"
#include "Misc/SecureHash.h"
#include "Misc/Crc.h"

#include "WaveVRSceneBlueprintFunctionLibrary.generated.h"

// BlueprintType enum only uint8 supported
UENUM(BlueprintType, Category = "WaveVR|Scene", meta=(DisplayName = "PerceptionTarget"))
enum class EWVRScenePerceptionTarget : uint8
{
	TwoDimentionPlane = 0,
	ThreeDimentionObject = 1,
	SceneMesh = 2,
	Max = 0xFF
};

UENUM(BlueprintType, Category = "WaveVR|Scene", meta = (DisplayName = "PerceptionTarget"))
enum class EWVRSceneMeshType : uint8
{
	VisualMesh = 0,
	ColliderMesh = 1,
	Max = 0xFF
};

UENUM(BlueprintType, Category = "WaveVR|Scene", meta=(DisplayName = "PerceptionState"))
enum class EWVRScenePerceptionState : uint8 {
	Empty = 0,
	Observing = 1,
	Paused = 2,
	Completed = 3,
	Max = 0xFF
};

UENUM(BlueprintType, Category = "WaveVR|Scene|Plane", meta=(DisplayName = "PlaneLabel"))
enum class EWVRScenePlaneLabel : uint8 {
	Unknown = 0,
	Floor = 1,
	Ceiling = 2,
	Wall = 3,
	Desk = 4,
	Couch = 5,
	Door = 6,
	Window = 7,
	Stage = 8,
	// sets Max meaning no filer
	Max = 0xFF
};

UENUM(BlueprintType, Category = "WaveVR|Scene|Plane", meta=(DisplayName = "PlaneType"))
enum class EWVRScenePlaneType : uint8 {
	Unknown = 0,
	HorizontalUpwardFacing = 1,
	HorizontalDownwardFacing = 2,
	Vertical = 3,
	// sets Max meaning no filer
	Max = 0xFF
};

#define WVR_SCENE_UID_SIZE   16

USTRUCT(BlueprintType, Category = "WaveVR|Scene", meta=(DisplayName = "SceneUuid"))
struct FWVRSceneUuid {
	GENERATED_USTRUCT_BODY()

	FWVRSceneUuid();

	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> data;

	UPROPERTY(BlueprintReadOnly)
	int32 uuidHash;

	bool operator==(const FWVRSceneUuid& rhs) const
	{
		for (int i = 0; i < 16; i++) {
			if (data[i] != rhs.data[i])
				return false;
		}
		return true;
	}
	bool operator!=(const FWVRSceneUuid& rhs) const
	{
		for (int i = 0; i < 16; i++) {
			if (data[i] != rhs.data[i])
				return true;
		}
		return false;
	}

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRSceneUuid& Tag) {
		uint32 hash = 0;
		for (int i = 0; i < 16; i++) {
			hash = HashCombine(hash, GetTypeHash(Tag.data[i]));
		}
		return hash;
	}
};

USTRUCT(BlueprintType, Category = "WaveVR|Scene", meta=(DisplayName = "SceneMesh"))
struct FWVRSceneMesh {
	GENERATED_USTRUCT_BODY()

	FWVRSceneMesh();

	UPROPERTY(BlueprintReadOnly)
	int64 meshBufferId;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRSceneMesh& Tag)
	{
		return ::GetTypeHash(Tag.meshBufferId);
	}
};

USTRUCT(BlueprintType, Category = "WaveVR|Scene|Plane", meta=(DisplayName = "ScenePlane"))
struct FWVRScenePlane {
	GENERATED_USTRUCT_BODY()

	FWVRScenePlane();

	UPROPERTY(BlueprintReadOnly)
	FWVRSceneUuid uuid;

	UPROPERTY(BlueprintReadOnly)
	FWVRSceneUuid parentUuid;

	UPROPERTY(BlueprintReadOnly)
	int64 meshBufferId;

	UPROPERTY(BlueprintReadOnly)
	FVector position;

	UPROPERTY(BlueprintReadOnly)
	FQuat rotation;

	UPROPERTY(BlueprintReadOnly)
	FVector2D extent;

	UPROPERTY(BlueprintReadOnly)
	EWVRScenePlaneType planeType;

	UPROPERTY(BlueprintReadOnly)
	EWVRScenePlaneLabel planeLabel;

	UPROPERTY(BlueprintReadOnly)
	FString semanticName;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRScenePlane& Tag)
	{
		return GetTypeHash(Tag.uuid);
	}
};


USTRUCT(BlueprintType, Category = "WaveVR|Scene|Object", meta = (DisplayName = "SceneObject"))
struct FWVRSceneObject {
	GENERATED_USTRUCT_BODY()

	FWVRSceneObject();

	UPROPERTY(BlueprintReadOnly)
	FWVRSceneUuid uuid;

	UPROPERTY(BlueprintReadOnly)
	FWVRSceneUuid parentUuid;

	UPROPERTY(BlueprintReadOnly)
	int64 meshBufferId;

	UPROPERTY(BlueprintReadOnly)
	FVector position;

	UPROPERTY(BlueprintReadOnly)
	FQuat rotation;

	UPROPERTY(BlueprintReadOnly)
	FVector extent;

	UPROPERTY(BlueprintReadOnly)
	FString semanticName;

	/** Used so we can have a TMap of this struct */
	FORCEINLINE friend uint32 GetTypeHash(const FWVRSceneObject& Tag)
	{
		return GetTypeHash(Tag.uuid);
	}
};


USTRUCT(BlueprintType, Category = "WaveVR|Scene|Plane", meta=(DisplayName = "PlaneFilter"))
struct FWVRScenePlaneFilter {
	GENERATED_USTRUCT_BODY()

	FWVRScenePlaneFilter();
	FWVRScenePlaneFilter(EWVRScenePlaneType type, EWVRScenePlaneLabel label);
	//explicit FORCEINLINE FPlaneFilter(EPlaneType type, EPlaneLabel label);
	static const FWVRScenePlaneFilter NoFilter;

	UPROPERTY(BlueprintReadWrite)
	EWVRScenePlaneType type;

	UPROPERTY(BlueprintReadWrite)
	EWVRScenePlaneLabel label;
};


USTRUCT(BlueprintType, Category = "WaveVR|Scene", meta=(DisplayName = "MeshBuffer"))
struct FWVRSceneMeshBuffer {
	GENERATED_USTRUCT_BODY()

	FWVRSceneMeshBuffer();
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> vertexBuffer;

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> indexBuffer;
};

UCLASS()
class WAVEVR_API UWaveVRSceneBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Start SceneMesh system."))
		static bool StartScene();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Stop SceneMesh system."))
		static void StopScene();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Start SceneMesh perception scan."))
		static bool StartScenePerception(EWVRScenePerceptionTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Stop SceneMesh perception scan."))
		static bool StopScenePerception(EWVRScenePerceptionTarget target);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get SceneMesh perception scan state."))
		static bool GetScenePerceptionState(EWVRScenePerceptionTarget target, EWVRScenePerceptionState& state);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get scene plane count by the filter."))
		static int GetScenePlaneCount(EWVRScenePlaneType type, EWVRScenePlaneLabel label, EWVRPoseOriginModel origin);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get scene planes."))
		static TArray<FWVRScenePlane> GetScenePlanes(EWVRScenePlaneType type, EWVRScenePlaneLabel label, EWVRPoseOriginModel origin);

	/** Returns true if the values are equal (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (ScenePlane)",
		CompactNodeTitle = "==",
		Tooltip = "Only check if A.UUID == B.UUID.",
		BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool EqualEqual_ScenePlane(const FWVRScenePlane& A, const FWVRScenePlane& B);

	/** Returns true if the values are not equal (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (ScenePlane)",
		CompactNodeTitle = "!=",
		Tooltip = "Only check if A.UUID != B.UUID.",
		BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool NotEqual_ScenePlane(const FWVRScenePlane& A, const FWVRScenePlane& B);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get scene scene meshes."))
		static TArray<FWVRSceneMesh> GetSceneMeshes(EWVRSceneMeshType type);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get vertices and indices data of Scene Plane, Scene Object, or Scene Mesh."))
		static FWVRSceneMeshBuffer GetSceneMeshBuffer(int64 meshBufferId, EWVRPoseOriginModel origin);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get scene object count."))
		static int GetSceneObjectCount(EWVRPoseOriginModel origin);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Get scene objects."))
		static TArray<FWVRSceneObject> GetSceneObjects(EWVRPoseOriginModel origin);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Make wireframe mesh by input buffer.  Use offset to adjust wireframe width and direction.  The original vertex will be added by the input offset to make the wireframe.  If return false, means vertices are too many or indices are not valid."))
		static bool MakeWireframe(TArray<FVector> verticesIn, TArray<int32> indicesIn, FVector offset, TArray<FVector>& verticesOut, TArray<int32>& indicesOut);

	/** Returns true if the values are equal (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (SceneUuid)", CompactNodeTitle = "==", BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool EqualEqual_SceneUuid(const FWVRSceneUuid& A, const FWVRSceneUuid& B);

	/** Returns true if the values are not equal (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (SceneUuid)", CompactNodeTitle = "!=", BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool NotEqual_SceneUuid(const FWVRSceneUuid& A, const FWVRSceneUuid& B);

	/** Returns Hex string of Uuid */
	UFUNCTION(BlueprintPure,
		Category = "WaveVR|Scene",
		meta = (DisplayName = "ToString (SceneUuid)", BlueprintThreadSafe))
		static FString ToString_SceneUuid(const FWVRSceneUuid& uuid);

	/** Returns true if the values are equal (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (SceneMesh)", CompactNodeTitle = "==", BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool EqualEqual_SceneMesh(const FWVRSceneMesh& A, const FWVRSceneMesh& B);

	/** Returns true if the values are not equal (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (SceneMesh)", CompactNodeTitle = "!=", BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool NotEqual_SceneMesh(const FWVRSceneMesh& A, const FWVRSceneMesh& B);

	/** Returns Hex string of Uuid */
	UFUNCTION(BlueprintPure,
		Category = "WaveVR|Scene",
		meta = (DisplayName = "ToString (FWVRSceneMesh)", BlueprintThreadSafe))
		static FString ToString_SceneMesh(const FWVRSceneMesh& uuid);

	/** Returns Hex string of Uuid */
	UFUNCTION(BlueprintPure,
		Category = "WaveVR|Scene",
		meta = (ToolTip = "Create Normals, Tangents, and UVs from Vertices and indices."))
		static void CreateMeshDataFromTriangles(const TArray<FVector> vertices, const TArray<int32> indices, TArray<FVector>& normals, TArray<FVector2D>& uvs, TArray<FLinearColor>& colors, TArray<FProcMeshTangent>& tangents);

	/** Returns true if the values are equal (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (SceneObject)",
		CompactNodeTitle = "==",
		Tooltip = "Only check if A.UUID == B.UUID.",
		BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool EqualEqual_SceneObject(const FWVRSceneObject& A, const FWVRSceneObject& B);

	/** Returns true if the values are not equal (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (SceneObject)",
		CompactNodeTitle = "!=",
		Tooltip = "Only check if A.UUID != B.UUID.",
		BlueprintThreadSafe), Category = "WaveVR|Scene")
		static bool NotEqual_SceneObject(const FWVRSceneObject& A, const FWVRSceneObject& B);

public:
	static bool LogWaveResult(const char* func, int wvr_result);
};
