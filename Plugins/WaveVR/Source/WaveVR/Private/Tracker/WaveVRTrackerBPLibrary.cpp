// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Tracker/WaveVRTrackerBPLibrary.h"

#include "Platforms/WaveVRLogWrapper.h"

EWaveVRTrackerId GetTrackerId(int id)
{
	if (id == 0) { return EWaveVRTrackerId::Tracker0; }
	if (id == 1) { return EWaveVRTrackerId::Tracker1; }
	if (id == 2) { return EWaveVRTrackerId::Tracker2; }
	if (id == 3) { return EWaveVRTrackerId::Tracker3; }
	if (id == 4) { return EWaveVRTrackerId::Tracker4; }
	if (id == 5) { return EWaveVRTrackerId::Tracker5; }
	if (id == 6) { return EWaveVRTrackerId::Tracker6; }
	if (id == 7) { return EWaveVRTrackerId::Tracker7; }
	if (id == 8) { return EWaveVRTrackerId::Tracker8; }
	if (id == 9) { return EWaveVRTrackerId::Tracker9; }
	if (id == 10) { return EWaveVRTrackerId::Tracker10; }
	if (id == 11) { return EWaveVRTrackerId::Tracker11; }
	if (id == 12) { return EWaveVRTrackerId::Tracker12; }
	if (id == 13) { return EWaveVRTrackerId::Tracker13; }
	if (id == 14) { return EWaveVRTrackerId::Tracker14; }
	return EWaveVRTrackerId::Tracker15;
}

void UWaveVRTrackerBPLibrary::StartTracker()
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return; }

	LOGD(LogWaveVRTrackerBPLibrary, "StartTracker()");
	pTracker->StartTracker();
}

void UWaveVRTrackerBPLibrary::StopTracker()
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return; }

	LOGD(LogWaveVRTrackerBPLibrary, "StopTracker()");
	pTracker->StopTracker();
}

EWaveVRTrackerStatus UWaveVRTrackerBPLibrary::GetTrackerStatus()
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return EWaveVRTrackerStatus::UNSUPPORT; }

	return pTracker->GetTrackerStatus();
}

bool UWaveVRTrackerBPLibrary::IsTrackerAvailable()
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->IsTrackerAvailable();
}

bool UWaveVRTrackerBPLibrary::IsTrackerConnected(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->IsTrackerConnected(trackerId);
}

EWaveVRTrackerRole UWaveVRTrackerBPLibrary::GetTrackerRole(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return EWaveVRTrackerRole::Undefined; }

	return pTracker->GetTrackerRole(trackerId);
}

bool UWaveVRTrackerBPLibrary::IsTrackerPoseValid(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->IsTrackerPoseValid(trackerId);
}

bool UWaveVRTrackerBPLibrary::GetTrackerDegreeOfFreedom(EWaveVRTrackerId trackerId, EWVR_DOF& dof)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->GetTrackerDegreeOfFreedom(trackerId, dof);
}

bool UWaveVRTrackerBPLibrary::GetTrackerPosition(EWaveVRTrackerId trackerId, FVector& outPosition)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->GetTrackerPosition(trackerId, outPosition);
}

bool UWaveVRTrackerBPLibrary::GetTrackerRotation(EWaveVRTrackerId trackerId, FQuat& outOrientation)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->GetTrackerRotation(trackerId, outOrientation);
}

EWVR_AnalogType UWaveVRTrackerBPLibrary::GetTrackerAnalogType(EWaveVRTrackerId trackerId, EWaveVRTrackerButton buttonId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return EWVR_AnalogType::None; }

	return pTracker->GetTrackerAnalogType(trackerId, buttonId);
}

bool UWaveVRTrackerBPLibrary::IsTrackerButtonPressed(EWaveVRTrackerId trackerId, EWaveVRTrackerButton buttonId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->IsTrackerButtonPressed(trackerId, buttonId);
}

bool UWaveVRTrackerBPLibrary::IsTrackerButtonTouched(EWaveVRTrackerId trackerId, EWaveVRTrackerButton buttonId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	return pTracker->IsTrackerButtonTouched(trackerId, buttonId);
}

FVector2D UWaveVRTrackerBPLibrary::GetTrackerButtonAxis(EWaveVRTrackerId trackerId, EWaveVRTrackerButton buttonId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return FVector2D::ZeroVector; }

	return pTracker->GetTrackerButtonAxis(trackerId, buttonId);
}

float UWaveVRTrackerBPLibrary::GetTrackerBatteryLife(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return 0; }

	return pTracker->GetTrackerBatteryLife(trackerId);
}

bool UWaveVRTrackerBPLibrary::TriggerTrackerVibration(EWaveVRTrackerId trackerId, int durationMicroSec, int frequency, float amplitude)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker == nullptr) { return false; }

	if (durationMicroSec < 0) { durationMicroSec = 500000; }
	if (frequency < 0) { frequency = 0; }
	return pTracker->TriggerTrackerVibration(trackerId, durationMicroSec, frequency, amplitude);
}

TArray<int> UWaveVRTrackerBPLibrary::GetTrackerExtendedData(EWaveVRTrackerId trackerId, int& validSize)
{
	int32_t timestamp = 0;
	return GetTrackerExtendedDataTimestamp(trackerId, validSize, timestamp);
}

TArray<int> UWaveVRTrackerBPLibrary::GetTrackerExtendedDataTimestamp(EWaveVRTrackerId trackerId, int& validSize, int& timestamp)
{
	TArray<int> exData;
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		uint64_t utimestamp = 0;
		int32_t* trackerExtData = pTracker->GetTrackerExtendedData(trackerId, &validSize, &utimestamp);
		timestamp = static_cast<int32_t>(utimestamp & 0x7FFFFFFF);
		if (validSize > 0)
		{
			for (int i = 0; i < validSize; i++)
			{
				//LOGI(LogWaveVRTrackerBPLibrary, "GetTrackerExtendedData() trackerId %d exData[%d] = %d", (uint8)trackerId, i, trackerExtData[i]);
				exData.Add(trackerExtData[i]);
			}
		}
	}

	return exData;
}

bool UWaveVRTrackerBPLibrary::GetTrackerDeviceName(EWaveVRTrackerId trackerId, FString& deviceName)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		bool ret = pTracker->GetTrackerDeviceName(trackerId, deviceName);
		//LOGI(LogWaveVRTrackerBPLibrary, "GetTrackerDeviceName() trackerId %d, nameSize: %d, deviceName: %s", (uint8)trackerId, nameSize, *deviceName);
		return ret;
	}
	return false;
}

bool UWaveVRTrackerBPLibrary::RegisterTrackerInfoCallback(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		LOGI(LogWaveVRTrackerBPLibrary, "RegisterTrackerInfoCallback() trackerId %d", (uint8)trackerId);
		return pTracker->RegisterTrackerInfoCallback(trackerId);
	}
	return false;
}

FString UWaveVRTrackerBPLibrary::GetTrackerCallbackInfo(EWaveVRTrackerId trackerId)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		return pTracker->GetTrackerCallbackInfo(trackerId);
	}
	return FString(TEXT(""));
}

bool UWaveVRTrackerBPLibrary::UnregisterTrackerInfoCallback()
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		LOGI(LogWaveVRTrackerBPLibrary, "UnregisterTrackerInfoCallback()");
		return pTracker->UnregisterTrackerInfoCallback();
	}
	return false;
}

void UWaveVRTrackerBPLibrary::SetFocusedTracker(EWaveVRTrackerId focusedTracker)
{
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		LOGI(LogWaveVRTrackerBPLibrary, "SetFocusedTracker() focusedTracker %d", (int)focusedTracker);
		pTracker->SetFocusedTracker((int)focusedTracker);
	}
}
bool UWaveVRTrackerBPLibrary::GetFocusedTracker(EWaveVRTrackerId& trackerId)
{
	int id = -1;
	WaveVRTrackerImpl* pTracker = WaveVRTrackerImpl::GetInstance();
	if (pTracker != nullptr)
	{
		id = pTracker->GetFocusedTracker();
		trackerId = GetTrackerId(id);
	}
	return (id >= 0 ? true : false);
}
