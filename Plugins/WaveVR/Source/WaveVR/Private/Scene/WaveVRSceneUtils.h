#pragma once

#include "Scene/WaveVRSceneBlueprintFunctionLibrary.h"
#include "Platforms/WaveVRAPIWrapper.h"

namespace scene {
	inline static void CopyUuid(FWVRSceneUuid& dst, const WVR_Uuid& src) {
		for (int i = 0; i < WVR_UUID_SIZE; i++)
			dst.data[i] = src.data[i];
		dst.uuidHash = (int)GetTypeHash(dst);
	}

	inline static void CopyUuid(WVR_Uuid& dst, const FWVRSceneUuid& src) {
		for (int i = 0; i < WVR_UUID_SIZE; i++)
			dst.data[i] = src.data[i];
	}

	inline static bool operator==(const FWVRSceneUuid& lhs, const FWVRSceneUuid& rhs) {
		for (int i = 0; i < WVR_UUID_SIZE; i++) {
			if (lhs.data[i] != rhs.data[i])
				return false;
		}
		return true;
	}
}
