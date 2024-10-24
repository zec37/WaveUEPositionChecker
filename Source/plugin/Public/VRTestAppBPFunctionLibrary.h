// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRTestAppBPFunctionLibrary.generated.h"


USTRUCT(BlueprintType)
struct FLevelDescription
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString pathName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString title;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString instructionEN;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString instructionTW;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool hidden = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool internal = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reserved0 = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString reserved1;
};

USTRUCT(BlueprintType)
struct FDeltaTimeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float deltaTime = 0.0f;;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float data = 0.0f;;
};

/**
 * 
 */
UCLASS()
class PLUGIN_API UVRTestAppBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "VRTestApp")
	static bool ParseJsonVRTestAppLevelData(FString json, TArray<FLevelDescription>& parseResult);
	
	UFUNCTION(BlueprintCallable, Category = "VRTestApp")
	static bool LoadFileToString(FString path, FString& data);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp")
	static bool SaveStringToFile(FString data, FString path);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp")
	static TArray<FDeltaTimeData> DropTimeDataArrayFromHead(TArray<FDeltaTimeData> deltaTimeArray, float duration);

	UFUNCTION(BlueprintCallable, Category = "VRTestApp")
	static TArray<float> AvgDataFromTimeDataArray(TArray<FDeltaTimeData> deltaTimeArray, int sampleCount, float duration, float max, bool useStdDevScaledMax, float StdDevScale, bool hasNegative);
};
