#include "Scene/WaveVRSceneProcedualMeshComponent.h"

#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(WVRScenePMC, Log, All);

UWaveVRSceneProcedualMeshComponent::UWaveVRSceneProcedualMeshComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bUseComplexAsSimpleCollision = false;
}
