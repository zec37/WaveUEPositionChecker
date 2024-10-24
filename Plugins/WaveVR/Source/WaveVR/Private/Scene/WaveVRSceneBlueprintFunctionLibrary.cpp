// "WaveVR SDK
// © 2022 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Scene/WaveVRSceneBlueprintFunctionLibrary.h"
#include "Scene/WaveVRSceneUtils.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Logging/LogMacros.h"

#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/WaveVRLogWrapper.h"
#include "WaveVRUtils.h"

#include "ProceduralMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(WVRSceneBPFL, Log, All);

#define WVR_EMAX(type, value) ((((int32)value) < 0xFF) ? ((type)value) : ((type)0x7FFFFFFF))
#define UE_EMAX(type, value) ((((int32)value) < 0x7FFFFFFF) ? ((type)value) : ((type)0xFF))

FWVRSceneUuid::FWVRSceneUuid() : uuidHash(0) {
	data.SetNumZeroed(WVR_UUID_SIZE);
}

using namespace wvr::utils;

FWVRSceneMesh::FWVRSceneMesh() : meshBufferId(0) {}

FWVRScenePlane::FWVRScenePlane()
	: meshBufferId(0)
	, position(FVector::ZeroVector)
	, rotation(FQuat::Identity)
	, extent(FVector2D::ZeroVector)
	, planeType(EWVRScenePlaneType::Max)
	, planeLabel(EWVRScenePlaneLabel::Max)
	, semanticName()
{}

FWVRSceneObject::FWVRSceneObject()
	: meshBufferId(0)
	, position(FVector::ZeroVector)
	, rotation(FQuat::Identity)
	, extent(FVector::ZeroVector)
	, semanticName()
{}

FWVRScenePlaneFilter::FWVRScenePlaneFilter() : type(EWVRScenePlaneType::Max), label(EWVRScenePlaneLabel::Max) {}
FWVRScenePlaneFilter::FWVRScenePlaneFilter(EWVRScenePlaneType t, EWVRScenePlaneLabel l) {
	type = t;
	label = l;
}

FWVRSceneMeshBuffer::FWVRSceneMeshBuffer() {}

const FWVRScenePlaneFilter FWVRScenePlaneFilter::NoFilter = FWVRScenePlaneFilter(EWVRScenePlaneType::Max, EWVRScenePlaneLabel::Max);


bool UWaveVRSceneBlueprintFunctionLibrary::StartScene() {
	return LogWaveResult("StartScene", WVR()->StartScene());
}

void UWaveVRSceneBlueprintFunctionLibrary::StopScene() {
	WVR()->StopScene();
}

bool UWaveVRSceneBlueprintFunctionLibrary::StartScenePerception(EWVRScenePerceptionTarget target) {
	return LogWaveResult("StartScenePerception", WVR()->StartScenePerception(WVR_EMAX(WVR_ScenePerceptionTarget, target)));
}

bool UWaveVRSceneBlueprintFunctionLibrary::StopScenePerception(EWVRScenePerceptionTarget target) {
	return LogWaveResult("StopScenePerception", WVR()->StopScenePerception(WVR_EMAX(WVR_ScenePerceptionTarget, target)));
}

bool UWaveVRSceneBlueprintFunctionLibrary::GetScenePerceptionState(EWVRScenePerceptionTarget target, EWVRScenePerceptionState& stateOut) {
	WVR_ScenePerceptionState state;
	auto e = WVR()->GetScenePerceptionState(WVR_EMAX(WVR_ScenePerceptionTarget, target), &state);
	stateOut = UE_EMAX(EWVRScenePerceptionState, state);
	return LogWaveResult("GetScenePerceptionState", e);
}

int UWaveVRSceneBlueprintFunctionLibrary::GetScenePlaneCount(EWVRScenePlaneType type, EWVRScenePlaneLabel label, EWVRPoseOriginModel origin) {
	WVR_ScenePlaneFilter filter{};
	filter.planeType = WVR_EMAX(WVR_ScenePlaneType, type);
	filter.planeLabel = WVR_EMAX(WVR_ScenePlaneLabel, label);

	uint32_t count = 0;
	uint32_t capacity = 0;
	WVR()->GetScenePlanes(&filter, 0, &count, (WVR_PoseOriginModel)origin, nullptr);
	return count;
}

TArray<FWVRScenePlane> UWaveVRSceneBlueprintFunctionLibrary::GetScenePlanes(EWVRScenePlaneType type, EWVRScenePlaneLabel label, EWVRPoseOriginModel origin) {
	WVR_ScenePlaneFilter filter{};
	filter.planeType = WVR_EMAX(WVR_ScenePlaneType, type);
	filter.planeLabel = WVR_EMAX(WVR_ScenePlaneLabel, label);

	uint32_t count = 0;
	uint32_t capacity = 0;
	WVR()->GetScenePlanes(&filter, 0, &count, (WVR_PoseOriginModel)origin, nullptr);
	TArray<FWVRScenePlane> planes = TArray<FWVRScenePlane>();
	if (count == 0)
		return planes;

	// TODO
	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;

	auto* ptr = new WVR_ScenePlane[count];
	capacity = count;
	WVR()->GetScenePlanes(&filter, capacity, &count, (WVR_PoseOriginModel)origin, ptr);
	for (uint32 i = 0; i < capacity; i++) {
		auto& wp = ptr[i];
		FWVRScenePlane up{};
		scene::CopyUuid(up.uuid, wp.uuid);
		scene::CopyUuid(up.parentUuid, wp.parentUuid);
		up.meshBufferId = wp.meshBufferId;
		up.position = CoordinateUtil::GetVector3(wp.pose.position, worldToMeters);
		up.rotation = CoordinateUtil::GetQuaternion(wp.pose.rotation) * FQuat(FRotator(0, 180, 0));  // rotate 180 for forward correction
		up.extent.X = wp.extent.width * worldToMeters;
		up.extent.Y = wp.extent.height * worldToMeters;
		up.planeType = UE_EMAX(EWVRScenePlaneType, wp.planeType);
		up.planeLabel = UE_EMAX(EWVRScenePlaneLabel, wp.planeLabel);
		up.semanticName = FString(ANSI_TO_TCHAR(wp.semanticName.name));
		planes.Add(up);
	}
	delete[] ptr;

	return planes;
}

bool UWaveVRSceneBlueprintFunctionLibrary::EqualEqual_ScenePlane(const FWVRScenePlane& A, const FWVRScenePlane& B) {
	return UWaveVRSceneBlueprintFunctionLibrary::EqualEqual_SceneUuid(A.uuid, B.uuid);
}

bool UWaveVRSceneBlueprintFunctionLibrary::NotEqual_ScenePlane(const FWVRScenePlane& A, const FWVRScenePlane& B) {
	return UWaveVRSceneBlueprintFunctionLibrary::NotEqual_SceneUuid(A.uuid, B.uuid);
}

TArray<FWVRSceneMesh> UWaveVRSceneBlueprintFunctionLibrary::GetSceneMeshes(EWVRSceneMeshType type) {
	uint32_t count = 0;
	uint32_t capacity = 0;
	WVR()->GetSceneMeshes(WVR_EMAX(WVR_SceneMeshType, type), 0, &count, nullptr);

	TArray<FWVRSceneMesh> meshes = TArray<FWVRSceneMesh>();
	if (count == 0)
		return meshes;

	auto ptr = new WVR_SceneMesh[count];
	capacity = count;
	WVR()->GetSceneMeshes(WVR_EMAX(WVR_SceneMeshType, type), capacity, &count, ptr);
	for (uint32 i = 0; i < capacity; i++) {
		FWVRSceneMesh mesh;
		mesh.meshBufferId = ptr[i].meshBufferId;
		meshes.Add(mesh);
	}
	delete [] ptr;
	ptr = nullptr;
	return meshes;
}

FWVRSceneMeshBuffer UWaveVRSceneBlueprintFunctionLibrary::GetSceneMeshBuffer(int64 meshBufferId, EWVRPoseOriginModel origin) {
	WVR_SceneMeshBuffer wb{};
	wb.indexBuffer = nullptr;
	wb.vertexBuffer = nullptr;
	wb.indexCapacityInput = 0;
	wb.vertexCapacityInput = 0;
	wb.indexCountOutput = 0;
	wb.vertexCountOutput = 0;
	uint32_t& countI = wb.indexCountOutput;
	uint32_t& countV = wb.vertexCountOutput;
	WVR()->GetSceneMeshBuffer(meshBufferId, (WVR_PoseOriginModel)origin, &wb);
	LOGD(WVRSceneBPFL, "GetSceneMeshBuffer() CI=%u CV=%u", countI, countV);

	FWVRSceneMeshBuffer ub{};
	if (countI == 0 || countV == 0) {
		return ub;
	}

	// TODO
	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;

	WVR_Vector3f* ptrV = new WVR_Vector3f[countV];
	int32 * ptrI = new int32[countI];
	wb.indexBuffer = (uint32*)ptrI;
	wb.vertexBuffer = ptrV;
	wb.indexCapacityInput = countI;
	wb.vertexCapacityInput = countV;
	WVR()->GetSceneMeshBuffer(meshBufferId, (WVR_PoseOriginModel)origin, &wb);

	//FVector* p = (FVector*)ptrV;
	//for (uint32 k = 0; k < countV; k++)
	//	LOGD(WVRSceneBPFL, "before [%04X] %5.3f, %5.3f, %5.3f", k, p[k].X, p[k].Y, p[k].Z);

	// No need change Index order
	ub.indexBuffer = TArray<int32>(ptrI, countI);
	ub.vertexBuffer.Empty();
	// Convert from WVR_Vector3f_t{ p.Y, p.Z, -p.X };
	for (uint32 i = 0; i < countV; i++) {
		auto v = CoordinateUtil::GetVector3(ptrV[i], worldToMeters);
		//LOGD(WVRSceneBPFL, "GetSceneMeshes idx=%u (%0.3f, %0.3f, %0.3f)", i, v.X, v.Y, v.Z);
		ub.vertexBuffer.Add(v);
	}

	//for (uint32 k = 0; k < countV; k++)
	//	LOGD(WVRSceneBPFL, "after [%04X] %5.3f, %5.3f, %5.3f", k, ub.vertexBuffer[k].X, ub.vertexBuffer[k].Y, ub.vertexBuffer[k].Z);

	delete[] ptrV;
	delete[] ptrI;
	ptrV = nullptr;
	ptrI = nullptr;
	return ub;
}

int UWaveVRSceneBlueprintFunctionLibrary::GetSceneObjectCount(EWVRPoseOriginModel origin) {
	uint32_t count = 0;
	uint32_t capacity = 0;
	WVR()->GetSceneObjects(0, &count, (WVR_PoseOriginModel)origin, nullptr);
	return count;
}

TArray<FWVRSceneObject> UWaveVRSceneBlueprintFunctionLibrary::GetSceneObjects(EWVRPoseOriginModel origin) {

	uint32_t count = 0;
	uint32_t capacity = 0;
	WVR()->GetSceneObjects(0, &count, (WVR_PoseOriginModel)origin, nullptr);
	TArray<FWVRSceneObject> objects = TArray<FWVRSceneObject>();
	if (count == 0)
		return objects;

	// TODO
	int worldToMeters = 100; //GWorld->GetWorldSettings()->WorldToMeters;

	auto* ptr = new WVR_SceneObject[count];
	capacity = count;
	WVR()->GetSceneObjects(capacity, &count, (WVR_PoseOriginModel)origin, ptr);
	for (uint32 i = 0; i < capacity; i++) {
		auto& wo = ptr[i];
		FWVRSceneObject uo{};
		scene::CopyUuid(uo.uuid, wo.uuid);
		scene::CopyUuid(uo.parentUuid, wo.parentUuid);
		uo.meshBufferId = wo.meshBufferId;
		uo.position = CoordinateUtil::GetVector3(wo.pose.position, worldToMeters);
		uo.rotation = CoordinateUtil::GetQuaternion(wo.pose.rotation) * FQuat(FRotator(0, 180, 0));  // rotate 180 for forward correction
		uo.extent.X = wo.extent.depth * worldToMeters;
		uo.extent.Y = wo.extent.width * worldToMeters;
		uo.extent.Z = wo.extent.height * worldToMeters;
		uo.semanticName = FString(ANSI_TO_TCHAR(wo.semanticName.name));

		objects.Add(uo);
	}
	delete[] ptr;

	return objects;
}


bool UWaveVRSceneBlueprintFunctionLibrary::MakeWireframe(TArray<FVector> verticesIn, TArray<int32> indicesIn, FVector offset, TArray<FVector>& verticesOut, TArray<int32>& indicesOut) {
	verticesOut.Empty();
	indicesOut.Empty();
	if ((indicesIn.Num() % 3 != 0)) {
		LOGE(WVRSceneBPFL, "MakeWireframe() index count is not a multiple of 3");
		return false;
	}

	//if ((verticesIn.Num() > 65536 / 2)) {
	//	LOGE(WVRSceneBPFL, "MakeWireframe() vertex count too many (> 32768)");
	//	return false;
	//}

	if (offset.Equals(FVector::ZeroVector)) {
		LOGE(WVRSceneBPFL, "MakeWireframe() offset is too small.");
		return false;
	}

	if (offset.Size() > 100) {
		LOGE(WVRSceneBPFL, "MakeWireframe() offset is too big (greater than 1m).");
		return false;
	}

	// double the vertex size;
	verticesOut = verticesIn;
	int N = verticesIn.Num();
	for (int i = 0; i < N; i++)
		verticesOut.Add(verticesIn[i] + offset);

	// Link the doubled vertex
	int triangleCount = indicesIn.Num() / 3;
	for (int i = 0; i < triangleCount; i++) {
		// Each triangle can create six wire link
		int i00 = indicesIn[i * 3];
		int i10 = indicesIn[i * 3 + 1];
		int i20 = indicesIn[i * 3 + 2];
		int i01 = i00 + N;  // doubled vertex
		int i11 = i10 + N;  // doubled vertex
		int i21 = i20 + N;  // doubled vertex

		indicesOut.Add(i00);
		indicesOut.Add(i01);
		indicesOut.Add(i11);
		indicesOut.Add(i00);
		indicesOut.Add(i11);
		indicesOut.Add(i10);

		indicesOut.Add(i10);
		indicesOut.Add(i11);
		indicesOut.Add(i21);
		indicesOut.Add(i10);
		indicesOut.Add(i21);
		indicesOut.Add(i20);

		indicesOut.Add(i20);
		indicesOut.Add(i21);
		indicesOut.Add(i01);
		indicesOut.Add(i20);
		indicesOut.Add(i01);
		indicesOut.Add(i00);
	}
	return true;
}


bool UWaveVRSceneBlueprintFunctionLibrary::EqualEqual_SceneUuid(const FWVRSceneUuid& A, const FWVRSceneUuid& B) {
	for (int i = 0; i < WVR_UUID_SIZE; i++) {
		if (A.data[i] != B.data[i])
			return false;
	}
	return true;
}

bool UWaveVRSceneBlueprintFunctionLibrary::NotEqual_SceneUuid(const FWVRSceneUuid& A, const FWVRSceneUuid& B) {
	for (int i = 0; i < WVR_UUID_SIZE; i++) {
		if (A.data[i] != B.data[i])
			return true;
	}
	return false;
}

FString UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneUuid(const FWVRSceneUuid& uuid) {
	uint8 bytes[WVR_UUID_SIZE] = { 0 };
	for (int i = 0; i < WVR_UUID_SIZE; i++) {
		bytes[i] = uuid.data[i];
	}
	uint32* v0 = (uint32*)(bytes);
	uint32* v1 = (uint32*)(bytes + 4);
	uint32* v2 = (uint32*)(bytes + 8);
	uint32* v3 = (uint32*)(bytes + 12);
	return FString::Printf(TEXT("%0X%0X%0X%0X"), *v0, *v1, *v2, *v3);
}

bool UWaveVRSceneBlueprintFunctionLibrary::EqualEqual_SceneMesh(const FWVRSceneMesh& A, const FWVRSceneMesh& B) {
	return A.meshBufferId == B.meshBufferId;
}

bool UWaveVRSceneBlueprintFunctionLibrary::NotEqual_SceneMesh(const FWVRSceneMesh& A, const FWVRSceneMesh& B) {
	return !(A.meshBufferId == B.meshBufferId);
}

FString UWaveVRSceneBlueprintFunctionLibrary::ToString_SceneMesh(const FWVRSceneMesh& mesh) {
	return FString::Printf(TEXT("%0l6" PRIX64), mesh.meshBufferId);
}


bool UWaveVRSceneBlueprintFunctionLibrary::LogWaveResult(const char* func, int wvr_result) {
	if (wvr_result == WVR_Success)
		return true;
	LOGE(WVRSceneBPFL, "call \"%s\" return WVR_Result=%d", USEWIDE(func), wvr_result);
	return false;
}

void UWaveVRSceneBlueprintFunctionLibrary::CreateMeshDataFromTriangles(const TArray<FVector> vertices, const TArray<int32> indices, TArray<FVector>& normals, TArray<FVector2D>& uvs, TArray<FLinearColor>& colors, TArray<FProcMeshTangent>& tangents) {
	const int Nv = vertices.Num();
	const int Nt = indices.Num() / 3;

	if (Nv == 0 || Nt * 3 != indices.Num()) {
		LOGE(WVRSceneBPFL, "ic=%d / 3 * 3 ! = %d", indices.Num(), indices.Num());
		return;
	}

	normals.Empty();
	normals.SetNum(Nv);

	tangents.Empty();
	tangents.SetNum(Nv);

	uvs.Empty();
	uvs.SetNum(Nv);

	colors.Empty();
	colors.SetNum(Nv);

	for (int i = 0; i < Nv; i++) {
		normals[i] = FVector::ZeroVector;
	}

	for (int i = 0; i < Nt; i++) {
		int i0 = indices[i * 3];
		int i1 = indices[i * 3 + 1];
		int i2 = indices[i * 3 + 2];

		auto& p0 = vertices[i0];
		auto& p1 = vertices[i1];
		auto& p2 = vertices[i2];

		auto v0 = p1 - p0;
		auto v1 = p2 - p1;
		auto n = FVector::CrossProduct(v0, v1);
		n.Normalize();

		normals[i0] += n;
		normals[i1] += n;
		normals[i2] += n;
	}

	for (int i = 0; i < Nv; i++) {
		normals[i].Normalize();

		tangents[i] = FProcMeshTangent(FVector::CrossProduct(normals[i], FVector::UpVector), false);

		colors[i].R = normals[i].X;
		colors[i].G = normals[i].Y;
		colors[i].B = normals[i].Z;
		colors[i].A = 1;
	}

	// Box Projection for UV unwrapping
	// Calculate a bounding box
	FVector minBox = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector maxBox = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0 ; i < Nv; i++)
	{
		FVector vertex = vertices[i];
		minBox.X = FMath::Min(minBox.X, vertex.X);
		minBox.Y = FMath::Min(minBox.Y, vertex.Y);
		minBox.Z = FMath::Min(minBox.Z, vertex.Z);
		maxBox.X = FMath::Max(maxBox.X, vertex.X);
		maxBox.Y = FMath::Max(maxBox.Y, vertex.Y);
		maxBox.Z = FMath::Max(maxBox.Z, vertex.Z);
	}

	FVector boxCenter = (minBox + maxBox) * 0.5f;
	LOGD(WVRSceneBPFL, "boxCenter=(%5.4f, %5.4f, %5.4f)", boxCenter.X, boxCenter.Y, boxCenter.Z);

	float xS = (maxBox.X - minBox.X) / 2;
	float yS = (maxBox.Y - minBox.Y) / 2;
	float zS = (maxBox.Z - minBox.Z) / 2;


	// let X side larger, then the corner angle will prefer other side.
	// Six face normals
	const FVector boxNormals[6] = {
		FVector( 1.00001f,  0.0f,  0.0f),  // +X
		FVector(-1.00001f,  0.0f,  0.0f),  // -X
		FVector( 0.0f,  1.0f,  0.0f),  // +Y
		FVector( 0.0f, -1.0f,  0.0f),  // -Y
		FVector( 0.0f,  0.0f,  1.0f),  // +Z
		FVector( 0.0f,  0.0f, -1.0f),  // -Z
	};

	//     0------X
	//    /|     /|
	//   / |    / |   Y
	//  Y------1  |   |
	//  |  Y---|--1   *--X
	//  | /    | /   /
	//  |/     |/   Z
	//  0------X         Only Z and Y face can have perfect uv,  X face cannot.

	// Right dir
	const FVector boxUDir[6] = {
		FVector( 0.0f, 0.0f, -1.0f),  // +X
		FVector( 0.0f, 0.0f,  1.0f),  // -X
		FVector( 1.0f, 0.0f,  0.0f),  // +Y
		FVector( 1.0f, 0.0f,  0.0f),  // -Y
		FVector( 1.0f, 0.0f,  0.0f),  // +Z
		FVector( 1.0f, 0.0f,  0.0f)   // -Z
	};

	const float boxUSize[6] = {
		zS, zS, xS, xS, xS, xS
	};

	// Up dir
	const FVector boxVDir[6] = {
		FVector(0.0f, 1.0f,  0.0f),  // +X
		FVector(0.0f, 1.0f,  0.0f),  // -X
		FVector(0.0f, 0.0f,  1.0f),  // +Y  // up-side-down face
		FVector(0.0f, 0.0f, -1.0f),  // -Y  // up-side-down face
		FVector(0.0f, 1.0f,  0.0f),  // +Z
		FVector(0.0f,-1.0f,  0.0f)   // -Z
	};

	const float boxVSize[6] = {
		yS, yS, zS, zS, yS, yS
	};

	// Calculate UV for each vertex
	for (int i = 0; i < Nv; i++)
	{
		const FVector& vertex = vertices[i];
		float minAngle = FLT_MAX;
		FVector cvDir = vertex - boxCenter;
		// Normalized according to box size.
		FVector cvNor = FVector(cvDir.X / xS, cvDir.Y / yS, cvDir.Z / zS);
		int dirId = 0;

		// Get angles to each face normal.  Find min value.
		for (int32 j = 0; j < 6; ++j)
		{
			float angle = FMath::Acos(FVector::DotProduct(cvNor, boxNormals[j]));

			if (angle < minAngle)
			{
				minAngle = angle;
				dirId = j;

				//LOGD(WVRSceneBPFL, "v=(%5.4f, %5.4f, %5.4f), boxNormals[%d]=(%5.4f, %5.4f, %5.4f), a=%f", cvNor.X, cvNor.Y, cvNor.Z, j, boxNormals[j].X, boxNormals[j].Y, boxNormals[j].Z, angle);
			}
		}

		// Calculate UV coordination by projection normal.
		float u = FVector::DotProduct(cvDir, boxUDir[dirId]) / boxUSize[dirId] / 2 + 0.5f;
		float v = FVector::DotProduct(cvDir, boxVDir[dirId]) / boxVSize[dirId] / 2 + 0.5f;

		uvs[i] = FVector2D(u, v);

		//LOGD(WVRSceneBPFL, "v=(%5.4f, %5.4f, %5.4f), uvs[%d]=(%5.4f, %5.4f) dirId=%d", vertex.X, vertex.Y, vertex.Z, i, u, v, dirId);
	}

	//LOGD(WVRSceneBPFL, "vc=%d,  ic=%d, nc=%d, colc=%d, tanc=%d", vertices.Num(), indices.Num(), normals.Num(), colors.Num(), tangents.Num());
}

bool UWaveVRSceneBlueprintFunctionLibrary::EqualEqual_SceneObject(const FWVRSceneObject& A, const FWVRSceneObject& B) {
	return A.uuid == B.uuid;
}

bool UWaveVRSceneBlueprintFunctionLibrary::NotEqual_SceneObject(const FWVRSceneObject& A, const FWVRSceneObject& B) {
	return !(A.uuid == B.uuid);
}



#undef WVR_EMAX
#undef UE_EMAX
