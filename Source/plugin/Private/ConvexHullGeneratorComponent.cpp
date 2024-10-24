// Fill out your copyright notice in the Description page of Project Settings.
#include "ConvexHullGeneratorComponent.h"
#include "Util/IndexUtil.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"

DEFINE_LOG_CATEGORY(ConvexHullG);

#ifdef LOGD
#undef LOGD
#endif
#define LOGD(Format, ...) UE_LOG(ConvexHullG, Display, TEXT(Format), ##__VA_ARGS__)

// Sets default values for this component's properties
UConvexHullGeneratorComponent::UConvexHullGeneratorComponent()
	: quantizer(2)
	, pointerCount(0)
	, meshComp(nullptr)
	, bShowMesh(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	meshComp = CreateDefaultSubobject<UProceduralMeshComponent>(FName("ProjectedPassthroughMesh"));
}


// Called when the game starts
void UConvexHullGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UConvexHullGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UConvexHullGeneratorComponent::Quantize(const FVector& v, float unit) {
	return FVector(
		FMath::RoundToFloat(v.X / unit) * unit,
		FMath::RoundToFloat(v.Y / unit) * unit,
		FMath::RoundToFloat(v.Z / unit) * unit);
}

int UConvexHullGeneratorComponent::AddPoint(const FVector& raw) {
	//LOGD("AddPoint() raw=(%.3f, %.3f, %.3f)", raw.X, raw.Y, raw.Z);
	// Record this component's world location and make cube.
	FVector tracking = refTransform.Inverse().TransformPosition(raw);
	auto z = refTransform.GetLocation();
	auto ret = list.AddUnique(Quantize(tracking, quantizer));  // every 2 cm a point

	if (ret >= 0) {
		auto v = list[ret];
		update = true;
		LOGD("Add one point %d(%.3f, %.3f, %.3f)", ret, v.X, v.Y, v.Z);
	}
	return ret;
}

void UConvexHullGeneratorComponent::ClearResult() {
	LOGD("ClearResult()");
	box = FOrientedBox3f();
	sphere = UE::Geometry::FSphere3f();
	list.Empty();
	verticesResult.Empty();
	indicesResult.Empty();
	update = false;
	pointerCount = 0;
}

void UConvexHullGeneratorComponent::SetReferenceTransform(const FTransform& ref) {
	auto l = ref.GetLocation();
	//LOGD("SetReferenceTransform() l=(%.3f, %.3f, %.3f)", l.X, l.Y, l.Z);
	refTransform = ref;
}

/*
	  Outter face                 Inner face
	   7---6     +z       or        3---2     -z
	   |\  |\                       |\  |\
	   4-\-5 \                      0-\-1 \
	    \ 3---2                      \ 7---6
	     \|   |                       \|   |
	      0---1  -z                    4---5  +z
	{ 0, 1, 2, 3 },     // back, -z
	{ 5, 4, 7, 6 },     // front, +z
	{ 4, 0, 3, 7 },     // left, -x
	{ 1, 5, 6, 2 },     // right, +x,
	{ 4, 5, 1, 0 },     // bottom, -y
	{ 3, 2, 6, 7 }      // top, +y
*/
const uint32_t ConvexHullMeshCreatorIndex[2 * 6 * 3] = {
	0, 1, 2, 0, 2, 3,     // back, -z
	5, 4, 7, 5, 7, 6,     // front, +z
	4, 0, 3, 4, 3, 7,     // left, -x
	1, 5, 6, 1, 6, 2,     // right, +x,
	4, 5, 1, 4, 1, 0,     // bottom, -y
	3, 2, 6, 3, 6, 7      // top, +y
};


bool UConvexHullGeneratorComponent::Solve(ECHG_TYPE type) {
	if (!update || list.Num() < 3) {
		LOGD("Number of points are not enough: %d", list.Num());
		return false;
	}
	update = false;

	switch (type) {
	case ECHG_TYPE::Box:
		return SolveForBox();
	//case ECHG_TYPE::Sphere:
	//	return SolveForSphere();
	case ECHG_TYPE::ConvexHull:
		return SolveForConvexHull();
	}
	return false;
}

bool UConvexHullGeneratorComponent::SolveForBox() {
	//LOGD("SolveForBox");

	minVolumeBox.Solve(list.Num(), [&](int32 Index) { return FVector3f(list[Index]); });
	if (!minVolumeBox.IsSolutionAvailable()) {
		LOGD("Fail to solve.");
		return false;
	}
	minVolumeBox.GetResult(box);

	verticesResult.Empty();
	for (int i = 0; i < 8; i++)
	{
		auto v = FVector(box.GetCorner(i));
		verticesResult.Add(v);
		//LOGD("Box[%d]=(%.3f, %.3f, %.3f)", i, v.X, v.Y, v.Z);
	}

	indicesResult.Empty();
	int indicesNum = (int)(sizeof(ConvexHullMeshCreatorIndex) / sizeof(*ConvexHullMeshCreatorIndex));
	for (int i = 0; i < indicesNum; i++)
	{
		indicesResult.Add(ConvexHullMeshCreatorIndex[i]);
	}

	//MakeProceduralMesh();

	//LOGD("Solved");
	return true;
}


bool UConvexHullGeneratorComponent::SolveForConvexHull() {
	//LOGD("SolveForConvexHull");
	
	convexHull.Solve(list.Num(), [&](int32 Index) { return FVector3f(list[Index]); });
	if (!convexHull.IsSolutionAvailable()) {
		LOGD("Fail to solve.");
		return false;
	}
	verticesResult.Empty();
	for (int i = 0; i < list.Num(); i++)
	{
		verticesResult.Add(list[i]);
	}

	auto indices = convexHull.GetTriangles();
	indicesResult.Empty();
	for (int i = 0; i < indices.Num(); i++)
	{
		indicesResult.Add(indices[i].A);
		indicesResult.Add(indices[i].B);
		indicesResult.Add(indices[i].C);
	}

	//LOGD("Solved");
	return true;
}

TArray<FVector> UConvexHullGeneratorComponent::GetSolvedVertices() {
	return verticesResult;
}

TArray<int32> UConvexHullGeneratorComponent::GetSolvedIndices() {
	return indicesResult;
}

// Only can used by box.  I don't know how to update mesh with new indices.
void UConvexHullGeneratorComponent::MakeProceduralMesh() {
	//LOGD("MakeProceduralMesh");
	TArray<int32> ois;
	TArray<FLinearColor> vertexColors;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FProcMeshTangent> tangents;

	tangents.SetNum(0);
	normals.SetNum(0);
	uvs.SetNum(0);
	vertexColors.SetNum(0);

	if (meshComp == nullptr) {
		LOGD("Create ProceduralMesh");
		if (meshComp == nullptr)
			return;
		meshComp->bUseAsyncCooking = false;
		meshComp->SetMaterial(0, previewMaterial);
		meshComp->CreateMeshSection_LinearColor(0, verticesResult, indicesResult, normals, uvs, vertexColors, tangents, /*true*/false);
		meshComp->SetWorldTransform(refTransform);
		meshComp->SetHiddenInGame(!bShowMesh);
	}
	else
	{
		// Always the same indices.
		meshComp->UpdateMeshSection_LinearColor(0, verticesResult, normals, uvs, vertexColors, tangents);
		meshComp->SetWorldTransform(refTransform);
		meshComp->SetHiddenInGame(!bShowMesh);
	}
}

void UConvexHullGeneratorComponent::ShowMesh(bool show) {
	bShowMesh = show;
	if (meshComp != nullptr)
		meshComp->SetHiddenInGame(!show);
}

