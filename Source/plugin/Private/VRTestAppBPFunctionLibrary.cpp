// Fill out your copyright notice in the Description page of Project Settings.

#include "VRTestAppBPFunctionLibrary.h"

#include "CoreMinimal.h"

#include "Json.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(VRTestApp, Display, All);

bool UVRTestAppBPFunctionLibrary::ParseJsonVRTestAppLevelData(FString jsonText, TArray<FLevelDescription>& parseResult)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(jsonText);
	TSharedPtr<FJsonObject> root;
	if (!FJsonSerializer::Deserialize(Reader, root)) {
		UE_LOG(VRTestApp, Error, TEXT("ParseJsonVRTestAppLevelData: Fail to be deserialized.  Please check format."));
		return false;
	}

	if (!root->HasTypedField<EJson::Array>("levels")) {
		UE_LOG(VRTestApp, Error, TEXT("ParseJsonVRTestAppLevelData: root not found"));
		return false;
	}

	parseResult.Empty();

	TArray<TSharedPtr<FJsonValue>> levels = root->GetArrayField("levels");
	for (int32 i = 0; i < levels.Num(); i++)
	{
		FLevelDescription desc {};
		TSharedPtr<FJsonObject> level = levels[i]->AsObject();
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: %d"), i);

		TSharedPtr<FJsonValue> pathName = level->TryGetField("pathName");
		if (!pathName.IsValid()) continue;
		desc.pathName = pathName->AsString();
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: pathName=%s"), *desc.pathName);

		TSharedPtr<FJsonValue> title = level->TryGetField("title");
		if (!title.IsValid()) continue;
		desc.title = title->AsString();
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: title=%s"), *desc.title);

		TSharedPtr<FJsonValue> instructionEN = level->TryGetField("instructionEN");
		if (!instructionEN.IsValid()) continue;
		desc.instructionEN = instructionEN->AsString();
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: instructionEN=%s"), *desc.instructionEN);

		TSharedPtr<FJsonValue> instructionTW = level->TryGetField("instructionTW");
		if (!instructionTW.IsValid()) continue;
		desc.instructionTW = instructionTW->AsString();
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: instructionTW=%s"), *desc.instructionTW);

		TSharedPtr<FJsonValue> hidden = level->TryGetField("hidden");
		if (hidden.IsValid())
			desc.hidden = hidden->AsBool();
		else
			desc.hidden = false;
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: hidden=%d"), desc.hidden);

		TSharedPtr<FJsonValue> internal = level->TryGetField("internal");
		if (internal.IsValid())
			desc.internal = internal->AsBool();
		else
			desc.internal = false;
		//UE_LOG(VRTestApp, Display, TEXT("ParseJsonVRTestAppLevelData: internal=%d"), desc.internal);

		TSharedPtr<FJsonValue> reserved0 = level->TryGetField("reserved0");
		if (reserved0.IsValid())
			desc.reserved0 = (float)reserved0->AsNumber();
		else
			desc.reserved0 = 0.0f;

		TSharedPtr<FJsonValue> reserved1 = level->TryGetField("reserved1");
		if (reserved1.IsValid())
			desc.reserved1 = reserved1->AsString();
		else
			desc.reserved1 = "";

		parseResult.Add(desc);
	}

	return true;
}

bool UVRTestAppBPFunctionLibrary::LoadFileToString(FString path, FString& data)
{
	auto p = FPaths::ProjectDir() + path;
	UE_LOG(VRTestApp, Display, TEXT("LoadFileToString(p=%s)"), *p);
	return FFileHelper::LoadFileToString(data, *p);
}

bool UVRTestAppBPFunctionLibrary::SaveStringToFile(FString data, FString path)
{
	auto p = FPaths::ProjectDir() + path;
	UE_LOG(VRTestApp, Display, TEXT("SaveStringToFile(p=%s)"), *p);
	return FFileHelper::SaveStringToFile(data, *p);
}

TArray<FDeltaTimeData> UVRTestAppBPFunctionLibrary::DropTimeDataArrayFromHead(TArray<FDeltaTimeData> deltaTimeArray, float duration)
{
	TArray<FDeltaTimeData> output;
	float accTime = 0;
	int copyStartIndex = 0;
	int dataCount = deltaTimeArray.Num();
	for (int i = dataCount - 1; i >= 0; i--) {
		accTime += deltaTimeArray[i].deltaTime;
		if (accTime > duration) {
			copyStartIndex = i;
			break;
		}
	}
	// Copy from start index to tail.
	for (int i = copyStartIndex; i < dataCount; i++) {
		output.Add(deltaTimeArray[i]);
	}
	return output;
}

TArray<float> UVRTestAppBPFunctionLibrary::AvgDataFromTimeDataArray(TArray<FDeltaTimeData> deltaTimeArray, int sampleCount, float duration, float max, bool useStdDevScaledMax, float StdDevScale, bool hasNegative)
{
	if (max <= 0)
		max = 1;

	TArray<float> output;
	int outputIndex = 0;
	int accFrameCount = 0;
	float accTime = 0;
	float accData = 0;
	int dataCount = deltaTimeArray.Num();
	float step = duration / sampleCount;
	for (int i = 0; i < dataCount; i++) {
		accTime += deltaTimeArray[i].deltaTime;
		accData += deltaTimeArray[i].data;
		accFrameCount += 1;
		bool calculateResult = false;

		if (i == dataCount - 1)
			calculateResult = true;
		if (accTime > step)
			calculateResult = true;

		if (calculateResult) {
			accData -= deltaTimeArray[i].data;
			accFrameCount -= 1;

			float avg = 0;
			if (accFrameCount > 0)
				avg = accData / accFrameCount;
			output.Add(avg);
			outputIndex++;

			accFrameCount = 1;
			accTime = accTime - step;
			accData = deltaTimeArray[i].data;
		}
	}

	// Fill up to sampleCount
	for (int i = outputIndex; i < sampleCount; i++) {
		output.Add(0);
	}

	if (useStdDevScaledMax && StdDevScale > 0) {
		// Find average
		float avg = 0;
		for (int i = 0; i < sampleCount; i++)
			avg += output[i] / sampleCount;

		// Find standard deviation
		float sumSqr = 0;
		for (int i = 0; i < sampleCount; i++) {
			float diff = output[i] - avg;
			float diffSqr = diff * diff;
			sumSqr += diffSqr / sampleCount;
		}
		float stdDev = FMath::Sqrt(sumSqr);
		float stdDevScaledMax = avg * (1 + stdDev * StdDevScale);

		max = FMath::Min(stdDevScaledMax, max);
	}

	float scale = hasNegative ? 0.5f : 1;
	float offset = hasNegative ? 0.5f : 0;
	for (int i = 0; i < sampleCount; i++)
		output[i] = FMath::Clamp<float>(output[i] / max * scale + offset, 0, 1);
	return output;
}
