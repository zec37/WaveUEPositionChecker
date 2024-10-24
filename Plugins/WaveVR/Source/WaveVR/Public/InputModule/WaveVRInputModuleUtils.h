// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "WaveVRInputModuleUtils.generated.h"

USTRUCT(BlueprintType)
struct FHandControl
{
	GENERATED_USTRUCT_BODY()

	/** True to enable hand control. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveVR|InputModule|Hand")
	bool UseHand = true;

	/** True to use default pinch value. Otherwise using the pinch strength and default threshold to judge if pinching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveVR|InputModule|Hand")
	bool UseDefaultPinch = false;
};
