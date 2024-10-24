#pragma once

/****************************************************
 *
 * FWaveVRTexturePool should only be included in CPP
 *
 ****************************************************/

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIUtilities.h"
#include "OpenGLDrv.h"
#include "wvr_render.h"

namespace Wave {
namespace Render {

struct FWaveVRRenderTextureInfo
{
	// Single eye size
	uint32 width;
	uint32 height;
	uint32 arraySize;  // Multiview layer
	ETextureCreateFlags flags;
	ETextureCreateFlags extraFlags;
	uint8 mips;
	uint8 format;
	uint8 numSamples;
	uint8 capacity;
	GLenum target;
	float pixelDensity;
	// Single eye scaled size
	uint32 scaledWidth;
	uint32 scaledHeight;
	// Real texture size
	uint32 renderWidth;
	uint32 renderHeight;

	bool createFromResource;  // If true, use the texture in wvrTextureQueue
	bool useUnrealTextureQueue;  // If true, we don't need create a base texture
	bool useWVRTextureQueue;  // Decided by TextureManager
	WVR_TextureQueueHandle_t wvrTextureQueue;  // Created by TextureManager
};


class FWaveVRTexturePool
{
public:
	// Create base texture
	FWaveVRTexturePool(const FWaveVRRenderTextureInfo& nativeInfo);

	virtual ~FWaveVRTexturePool();
	void ReleaseTextures();

public:
	FTextureRHIRef CreateBaseColor();
	FTextureRHIRef CreateBaseDepth(const FWaveVRRenderTextureInfo& engineInfo);
	void CreateColorInPool(uint32 index);
	void CreateDepthInPool(const FWaveVRRenderTextureInfo& engineInfo, uint32 index);

public:
	void MakeAlias(int index, GLuint resource = 0);
	uint32 MakeDepthAlias(int index, GLuint resource = 0);
	uint32 FindIndexByGLId(GLuint id) const;
	uint32 FindIndexByTexture(FOpenGLTexture* OpenGLTexture) const;
	void UpdatePixelDensityInPool(const FWaveVRRenderTextureInfo& engineInfo);

public:
	// Get
	inline const FWaveVRRenderTextureInfo GetInfo() const { return textureInfo; }
	const TRefCountPtr<FOpenGLTexture> GetGLTexture(uint32 index) const;
	FTextureRHIRef GetRHIColor(uint32 index) const;
	FTextureRHIRef GetRHIDepth(uint32 index) const;

private:
	FWaveVRTexturePool(const FWaveVRTexturePool &) = delete;
	FWaveVRTexturePool(FWaveVRTexturePool &&) = delete;
	FWaveVRTexturePool &operator=(const FWaveVRTexturePool &) = delete;

private:
	FTextureRHIRef mBaseColor;
	FTextureRHIRef mBaseDepth;

	TArray<FTextureRHIRef> mColorPool;
	TArray<FTextureRHIRef> mDepthPool;

	FWaveVRRenderTextureInfo textureInfo;
};


}  // namespace Render
}  // namespace Wave
