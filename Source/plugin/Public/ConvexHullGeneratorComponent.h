// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "MinVolumeBox3.h"
#include "MinVolumeSphere3.h"
#include "CompGeom/ConvexHull3.h"
#include "VectorTypes.h"

#include "ProceduralMeshComponent.h"
#include "Logging/LogMacros.h"

#include "ConvexHullGeneratorComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ConvexHullG, Log, All);

using namespace UE::Geometry;

UENUM(BlueprintType, Category = "VRTestApp|ProjPT")
enum class ECHG_TYPE : uint8
{
	Box = 0,
	//Sphere = 1,  // We need sphere's mesh generator.  The solved result only have center anc diameter.
	ConvexHull = 2
};

UCLASS( ClassGroup=(VRTestApp), meta=(BlueprintSpawnableComponent) )
class PLUGIN_API UConvexHullGeneratorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConvexHullGeneratorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Add a point"))
		int AddPoint(const FVector& point);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Remove old result"))
		void ClearResult();

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Set the origin point (i.g. Pawn's position)"))
		void SetReferenceTransform(const FTransform& ref);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Build a mesh with exist points if possible.  Input boxOrCH=0 for min volume box.  Input boxOrCH=1 for a convex hull mesh."))
		bool Solve(ECHG_TYPE type);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Get the Vertices"))
		TArray<FVector> GetSolvedVertices();

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Get the Incies"))
		TArray<int32> GetSolvedIndices();

	UFUNCTION(BlueprintCallable, Category = "VRTestApp|ProjPT",
		meta = (ToolTip = "Start to show passthrough by the mesh"))
		void ShowMesh(bool show);

	UPROPERTY(EditAnywhere, Category = "VRTestApp|ProjPT")
		UMaterialInterface* previewMaterial;

	UPROPERTY(EditAnywhere, Category = "VRTestApp|ProjPT")
		int quantizer;

	UPROPERTY(BlueprintReadOnly, Category = "VRTestApp|ProjPT")
		int pointerCount;

private:
	bool SolveForBox();
	bool SolveForConvexHull();

	FVector Quantize(const FVector& v, float unit);

	void MakeProceduralMesh();

private:
	FMinVolumeBox3f minVolumeBox;
	FMinVolumeSphere3f minVolumeSphere;
	FConvexHull3f convexHull;
	FOrientedBox3f box;
	UE::Geometry::FSphere3f sphere;
	TArray<FVector> list;
	TArray<FVector> verticesResult;
	TArray<int32_t> indicesResult;
	FTransform refTransform;
	bool update = false;
	UProceduralMeshComponent* meshComp;
	bool bShowMesh;
};

extern const uint32_t ConvexHullMeshCreatorIndex[36];
