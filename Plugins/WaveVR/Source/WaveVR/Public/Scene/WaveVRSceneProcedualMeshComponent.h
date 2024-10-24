// "WaveVR SDK
// © 2022 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "WaveVRSceneProcedualMeshComponent.generated.h"

UCLASS(
	ClassGroup = "WaveVR|Scene",
	meta = (BlueprintSpawnableComponent, DisplayName = "SceneMeshComponent"))
	class WAVEVR_API UWaveVRSceneProcedualMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UWaveVRSceneProcedualMeshComponent(const FObjectInitializer& ObjectInitializer);
};
