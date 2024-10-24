
#include "WaveVRTexturePool.h"
#include "WaveVRRender.h"
#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/WaveVRLOGWrapper.h"

#ifndef PTR_TO_INT
#if PLATFORM_ANDROID && PLATFORM_ANDROID_ARM //(32bit)
#define PTR_TO_INT(x) x
#define INT_TO_PTR(x) x
#else //PLATFORM_ANDROID_ARM64 || PLATFORM_WINDOWS || WITH_EDITOR  /*Other 64bit*/
#define PTR_TO_INT(x) \
  (int)(long long)(x)
#define INT_TO_PTR(x) \
  (long long)(x)
#endif
#endif
#define NATIVE_RESOURCE_TO_INT(x) \
  static_cast<uint32_t>(*(GLuint*)(x))

//#if PLATFORM_ANDROID
//#define WAVEVR_LOG_SHOW_ALL_ENTRY 1
//#endif

#if defined(WAVEVR_LOG_SHOW_ALL_ENTRY) && (PLATFORM_ANDROID || PLATFORM_WINDOWS)
#if WAVEVR_LOG_SHOW_ALL_ENTRY && !WITH_EDITOR
#ifdef LOG_FUNC
#undef LOG_FUNC
#endif
#define LOG_FUNC() LOGD(WVRRenderTP, "%s", WVR_FUNCTION_STRING) // ;FDebug::DumpStackTraceToLog();
#endif
#define LOG_STACKTRACE() FDebug::DumpStackTraceToLog()
#endif //PLATFORM_ANDROID


#ifdef WVR_SCOPED_NAMED_EVENT
#undef WVR_SCOPED_NAMED_EVENT
#endif
#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG) && !WITH_EDITOR
#define WVR_SCOPED_NAMED_EVENT(name, color) SCOPED_NAMED_EVENT(name, color)
#else
#define WVR_SCOPED_NAMED_EVENT(name, color)
#endif

namespace Wave {
namespace Render {

DEFINE_LOG_CATEGORY_STATIC(WVRRenderTP, Display, All);

#ifdef LogTextureInfo
#undef LogTextureInfo
#endif
#define LogTextureInfo(title, info) \
	LOGI(WVRRenderTP, title " TextureInfo width=%u height=%u PD=%f sWidth=%u sHeight=%u array=%u " \
		"rWidth=%u rHeight=%u samples=%u flags=%llu extFlags=%llu format=%u mips=%d " \
		"target=%d capacity=%u useUnrealTextureQueue=%d createFromResource=%d queue=%p", \
		info.width, info.height, info.pixelDensity, info.scaledWidth, info.scaledHeight,info.arraySize, \
		info.renderWidth, info.renderHeight, info.numSamples, info.flags, info.extraFlags, info.format, info.mips, \
		info.target, info.capacity, info.useUnrealTextureQueue, info.createFromResource, info.wvrTextureQueue);

void FWaveVRTexturePool::UpdatePixelDensityInPool(const FWaveVRRenderTextureInfo& engineInfo)
{
	WVR_SCOPED_NAMED_EVENT(UpdatePixelDensityInPool, FColor::Orange);
	LOG_FUNC();
	LogTextureInfo("UpdatePixelDensityInPool", engineInfo);

	textureInfo.pixelDensity = engineInfo.pixelDensity;
	textureInfo.scaledWidth = engineInfo.scaledWidth;
	textureInfo.scaledHeight = engineInfo.scaledHeight;
	textureInfo.renderWidth = engineInfo.renderWidth;
	textureInfo.renderHeight = engineInfo.renderHeight;

	if (textureInfo.useUnrealTextureQueue && textureInfo.createFromResource && textureInfo.wvrTextureQueue) {
		// No base color texture, use native created resource from texture queue.
		if (!mColorPool.IsEmpty()) {
			for (int i = 0; i < textureInfo.capacity; i++) {
				// When Refs count to zero, the texture's destructor will call Release() itself.  We don't have to call Release().
				//mColorPool[i].Release();
				LOGD(WVRRenderTP, "SafeRelease mColorPool[%d].GetRefCount()=%d", i, mColorPool[i].GetRefCount());
				mColorPool[i].SafeRelease();
			}
		}

		if (textureInfo.wvrTextureQueue != nullptr)
		{
			//LOGD(WVRRenderTP, "WVR_ReleaseTextureQueue()");
			WVR()->ReleaseTextureQueue(textureInfo.wvrTextureQueue);
			textureInfo.wvrTextureQueue = nullptr;
		}

		WVR_TextureTarget wvrTextureTarget;
		if (textureInfo.arraySize > 1)
			wvrTextureTarget = WVR_TextureTarget_2D_ARRAY;
		else
			wvrTextureTarget = WVR_TextureTarget_2D;
		textureInfo.wvrTextureQueue = WVR()->ObtainTextureQueue(wvrTextureTarget, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, textureInfo.renderWidth, textureInfo.renderHeight, 0);
		//GEngine->ForceGarbageCollection();
	}

	if (!textureInfo.useUnrealTextureQueue) {
		LOGD(WVRRenderTP, "update the base texture");
		CreateBaseColor();
	} else {
		// If useUnrealTextureQueue, no need to create a base texture, and each texture will be allocated during AllocateColorTexture.
	}
}

FWaveVRTexturePool::FWaveVRTexturePool(const FWaveVRRenderTextureInfo& info)
	: mBaseColor(nullptr)
	, mBaseDepth()
	, textureInfo(info)
{
	LOG_FUNC();
	LogTextureInfo("FWaveVRTexturePool", info);

	// This is a pool.
	check(textureInfo.capacity);

	mColorPool.Reserve(textureInfo.capacity);
	mDepthPool.Reserve(textureInfo.capacity);
	for (int i = 0; i < textureInfo.capacity; i++) {
		mColorPool.Add(nullptr);
		mDepthPool.Add(nullptr);
	}

}

FWaveVRTexturePool::~FWaveVRTexturePool()
{
	LOG_FUNC();
	ReleaseTextures();
}

// Used with old unreal WVR texture queue.  Not follow WVR's texture queue
FTextureRHIRef FWaveVRTexturePool::CreateBaseColor()
{
	LOG_FUNC();

	check(!textureInfo.useUnrealTextureQueue);

	const auto& info = textureInfo;

#if 0 // [UE_5.1] Keep the old implementation for reference.
	// Create base texture which will give to SceneViewport of UnrealEngine.
	//if (!info.useUnrealTextureQueue)
	mBaseColor = new FOpenGLTexture2D(GLRHI, 0, info.target, GL_NONE/*GL_COLOR_ATTACHMENT0*/, info.renderWidth, info.renderHeight, 0, 1, 1, info.numSamples, info.arraySize, EPixelFormat(info.format), false, false, info.flags, FClearValueBinding::Black);
	mBaseColor->SetAliased(true);
	OpenGLTextureAllocated(mBaseColor, info.flags);  // Do what is done in InitializeGLTexture
#endif
	const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("RenderTarget_BaseColor"), info.renderWidth, info.renderHeight, EPixelFormat(info.format))
		.SetNumMips(1)
		.SetNumSamples(1)
		.SetFlags(info.flags)
		.SetClearValue(FClearValueBinding::Black)
		.DetermineInititialState();
	mBaseColor = RHICreateTexture(CreateDesc);
	LOGD(WVRRenderTP, "mBaseColor tid=%u", NATIVE_RESOURCE_TO_INT(mBaseColor->GetNativeResource()));

	return mBaseColor;
}

FTextureRHIRef FWaveVRTexturePool::CreateBaseDepth(const FWaveVRRenderTextureInfo& engineInfo)
{
	LOG_FUNC();

#if 0
	// XXX Assert: Ensure condition failed : (DepthStencilRenderTarget.DepthStencilTarget->GetFlags() & TexCreate_InputAttachmentRead) != 0
	uint32_t flags = engineInfo.flags | engineInfo.extraFlags;
	// Not support arraySize
	return GLRHI->CreateOpenGLRHITextureOnly(engineInfo.renderWidth, engineInfo.renderHeight, false, engineInfo.arraySize > 1, false, engineInfo.format, 1, 1, engineInfo.arraySize, flags, FClearValueBinding::DepthFar);
#else
	int arraySize = engineInfo.arraySize;
	bool isArray = arraySize > 1;

	// target is not GL_TEXTURE_2D_MULTISAMPLE.  Because NumSamplesTileMem is not 1.
	GLenum target = isArray ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
	ETextureCreateFlags flags = ETextureCreateFlags(engineInfo.flags | engineInfo.extraFlags);
	FTextureRHIRef depth = nullptr;
	if (isArray) {
		const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2DArray(TEXT("RenderTarget_BaseDepth_2DArray"), engineInfo.renderWidth, engineInfo.renderHeight, EPixelFormat::PF_DepthStencil/*PF_X24_G8*/)
			.SetNumMips(1)
			.SetNumSamples(engineInfo.numSamples)
			.SetFlags(flags)
			.SetArraySize(engineInfo.arraySize)
			.SetClearValue(FClearValueBinding::DepthFar);
			//.DetermineInititialState();
		depth = RHICreateTexture(CreateDesc);
		//LOGD(WVRRenderTP, "mBaseDepth 2DArray tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));  // Must be 0
	} else {
		const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("RenderTarget_BaseDepth_2D"), engineInfo.renderWidth, engineInfo.renderHeight, EPixelFormat::PF_DepthStencil/*PF_X24_G8*/)
			.SetNumMips(1)
			.SetNumSamples(engineInfo.numSamples)
			.SetFlags(flags)
			.SetClearValue(FClearValueBinding::DepthFar)
			.DetermineInititialState();
		depth = RHICreateTexture(CreateDesc);
		//LOGD(WVRRenderTP, "mBaseDepth 2D tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));  // Must be 0
	}
#endif
	LOGD(WVRRenderTP, "mBaseDepth tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));  // Must be 0
	return depth;
}

static FTextureRHIRef CreateTexture2D(const FWaveVRRenderTextureInfo& info, GLuint resource = 0)
{
	LOG_FUNC();
	FTextureRHIRef Texture = nullptr;
	if (PLATFORM_WINDOWS) {
		const FRHITextureCreateDesc CreateDesc =
			FRHITextureCreateDesc::Create2D(TEXT("CreateColorInPool"), info.renderWidth, info.renderHeight, EPixelFormat(info.format))
			.SetInitialState(ERHIAccess::CopySrc)
			.SetFlags(info.flags);
		Texture = RHICreateTexture(CreateDesc);
	} else if (PLATFORM_ANDROID) {
		auto GLRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
		check(GLRHI);

		if (info.arraySize > 1) {
			if (resource == 0) {
#if 0 // [UE_5.1] Keep the old implementation for reference.
				 Not Work ?  texture release it self?  modify the texture as FTexture2DRHIRef.  Need verify.
				FTexture2DRHIRef texture = new FOpenGLTexture2D(
					GLRHI, 0, info.target, GL_NONE, info.renderWidth, info.renderHeight, 0,
					info.mips, 1, info.numSamples/*SamplesTileMem*/,
					info.arraySize, EPixelFormat(info.format), false, false, info.flags, FClearValueBinding::Black);
				GLRHI->InitializeGLTexture(
					texture, info.renderWidth, info.renderHeight, false, true/*Texture3D*/, false,
					EPixelFormat(info.format), info.mips, 1/*numSamples*/,
					info.arraySize, info.flags, FClearValueBinding::Black, nullptr);
				return texture;
#endif
				const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2DArray(TEXT("CreateColorInPool_2DArray"), info.renderWidth, info.renderHeight, EPixelFormat(info.format))
					.SetNumMips(info.mips)
					.SetNumSamples(1)
					.SetFlags(info.flags)
					.SetClearValue(FClearValueBinding::Black)
					.DetermineInititialState();
				Texture = RHICreateTexture(CreateDesc);
			} else {
				// SamplesTileMem is a don't care value
				Texture = GLRHI->RHICreateTexture2DArrayFromResource(
					EPixelFormat(info.format), info.renderWidth, info.renderHeight,
					info.arraySize,
					1/*info.mips*/, 1/*info.numSamples*/, 1/*SamplesTileMem*/,
					FClearValueBinding::Black, resource, info.flags);
			}
		} else {
			if (resource == 0) {
				const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("CreateColorInPool_2D"), info.renderWidth, info.renderHeight, EPixelFormat(info.format))
					.SetNumMips(1/*info.mips*/)
					.SetNumSamples(info.numSamples)
					.SetFlags(info.flags)
					.SetClearValue(FClearValueBinding::Black)
					.SetInitialState(ERHIAccess::CopySrc);
				Texture = RHICreateTexture(CreateDesc);
			} else {
				// SamplesTileMem is a don't care value
				Texture = GLRHI->RHICreateTexture2DFromResource(
					EPixelFormat(info.format), info.renderWidth, info.renderHeight,
					1/*info.mips*/, 1/*info.numSamples*/, 1/*SamplesTileMem*/,
					FClearValueBinding::Black, resource, info.flags);
			}
		}
	}
	return Texture;
}

// Only used to create the textures for WVR submit.  Textures are following the WVR settings.
void FWaveVRTexturePool::CreateColorInPool(uint32 index)
{
	LOG_FUNC();

	check(index <= textureInfo.capacity);

	const auto info = textureInfo;

	// 4 cases
	if (textureInfo.useUnrealTextureQueue && textureInfo.createFromResource) {
		// Use textures in wvrTextureQueue and not use a base texture to alias.
		WVR_TextureParams_t params;
		check(textureInfo.wvrTextureQueue);
		params = WVR()->GetTexture(info.wvrTextureQueue, index); //get texture id, target, layout
		mColorPool[index] = CreateTexture2D(info, (GLuint)PTR_TO_INT(params.id));
		//LOGD(WVRRenderTP, "CreateColorInPool WVRTextureId(%u) mColorPool[%d] tid is (%u)", (GLuint)PTR_TO_INT(params.id), index, NATIVE_RESOURCE_TO_INT(mColorPool[index]->GetNativeResource()));
	} else if (!textureInfo.useUnrealTextureQueue && textureInfo.createFromResource) {
		// Use textures in wvrTextureQueue and use a base texture to alias.
		check(textureInfo.wvrTextureQueue);
		check(mBaseColor);
		check(index == 0);

		for (int i = 0; i < info.capacity; i++) {
			WVR_TextureParams_t params = WVR()->GetTexture(info.wvrTextureQueue, info.capacity - 1 - i /* damn it. wvr is inversed */);

			mColorPool[i] = CreateTexture2D(info, (GLuint)PTR_TO_INT(params.id));
		}
	} else if (!textureInfo.useUnrealTextureQueue && !textureInfo.createFromResource) {
		// As the old way
		// Create texture in unreal and use a base texture to alias.
		check(mBaseColor);
		check(index == 0);

		for (int i = 0; i < info.capacity; i++) {
			mColorPool[i] = CreateTexture2D(info);
			MakeAlias(i);
		}
		//void ** textures = new void *[info.capacity];
		//for (int i = 0; i < info.capacity; i++) {
		//	textures[i] = (void *)PTR_TO_INT(*(GLuint *)(mColorPool[i]->GetNativeResource()));
		//	LOGI(WVRRenderTP, "WVR_TextureParams_t[%d]=%p", i, textures[i]);
		//}
		//textureInfo.wvrTextureQueue = WVR()->StoreRenderTextures(textures, info.capacity, WVR_Eye_Left);

	} else if (textureInfo.useUnrealTextureQueue && !textureInfo.createFromResource) {
		// Create texture in unreal and not use a base texture to alias.
		mColorPool[index] = CreateTexture2D(info);
	} else {
		// Impossible
		check(false);
	}
}

static FTextureRHIRef CreateDepthTexture2D(const FWaveVRRenderTextureInfo& info)
{
	LogTextureInfo("CreateDepthTexture2D", info);

	int arraySize = info.arraySize;
	bool isArray = arraySize > 1;
	ETextureCreateFlags flags = ETextureCreateFlags(info.flags | info.extraFlags);  // extraFlags 524 = TexCreate_DepthStencilTargetable 4 | TexCreate_ShaderResource 8 | TexCreate_InputAttachmentRead 512

	FTextureRHIRef depth = nullptr;
	if (isArray) {
		const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2DArray(TEXT("CreateDepthTexture2D_2DArray"), info.renderWidth, info.renderHeight, (EPixelFormat)info.format)
			.SetNumMips(1)
			.SetNumSamples(info.numSamples)
			.SetFlags(flags)
			.SetClearValue(FClearValueBinding::DepthFar)
			.DetermineInititialState();
		depth = RHICreateTexture(CreateDesc);
		LOGD(WVRRenderTP, "CreateDepthTexture2D 2DArray tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));
	}
	else {
		const FRHITextureCreateDesc CreateDesc = FRHITextureCreateDesc::Create2D(TEXT("CreateDepthTexture2D_2D"), info.renderWidth, info.renderHeight, (EPixelFormat)info.format)
			.SetNumMips(1)
			.SetNumSamples(info.numSamples)
			.SetFlags(flags)
			.SetClearValue(FClearValueBinding::DepthFar)
			.DetermineInititialState();
		depth = RHICreateTexture(CreateDesc);
		LOGD(WVRRenderTP, "CreateDepthTexture2D 2D tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));
	}
	LOGD(WVRRenderTP, "CreateDepthTexture2D tid=%u", NATIVE_RESOURCE_TO_INT(depth->GetNativeResource()));
	return depth;
}

void FWaveVRTexturePool::CreateDepthInPool(const FWaveVRRenderTextureInfo& engineInfo, uint32 index)
{
	LOG_FUNC();
	check(index == 0);

	const auto& info = textureInfo;

	if (!mBaseDepth.IsValid()) {
		mBaseDepth = CreateBaseDepth(engineInfo);
	}

	// Depth
	if (!mDepthPool.IsEmpty()) {
		for (int i = 0; i < info.capacity; i++) {
			// When Refs count to zero, the texture's destructor will call Release() itself.  We don't have to call Release().
			//mDepthPool[i]->Release();
			if(mDepthPool[i].IsValid()) {
				LOGD(WVRRenderTP, "SafeRelease mDepthPool[%d].GetRefCount()=%d", i, mDepthPool[i].GetRefCount());
				mDepthPool[i].SafeRelease();
			}
		}
	}
	for (int i = 0; i < info.capacity; i++) {
		mDepthPool[i] = CreateDepthTexture2D(engineInfo);
	}

	MakeDepthAlias(2);
}

// Very confusable here.
// Idx could be used by the target or the base.  It's depended on resource.
void FWaveVRTexturePool::MakeAlias(int index, GLuint resource)
{
	LOG_FUNC();
	check(mColorPool[index].IsValid());

	FOpenGLTexture* target = GetOpenGLTextureFromRHITexture(mColorPool[index]);

	FOpenGLTexture* base = mBaseColor.IsValid() ? GetOpenGLTextureFromRHITexture(mBaseColor) : target;

	// mBaseColor of course is aliased.  target could be aliased or not.
	if (mBaseColor.IsValid())
	{
		if (resource == 0) {
			// Has mBase and use idx target.
			// Alias the the target resource from texture pool
			base->AliasResources(*target);
			//LOGV(WVRRenderTP, "MakeAlias(%d, 0) -> target %u", idx, target->Resource);
		} else {
			// For using unreal texture queue, the resource not match the wvr queue's idx.
			if (base->GetResource() != resource) {
				//LOGV(WVRRenderTP, "MakeAlias(%d, %u) -> originalResource %u -> %u", idx, resource, base->Resource, resource);
				base->SetResource(resource);
			} else {
				//LOGV(WVRRenderTP, "MakeAlias(%d, %u) noop", idx, resource);
			}
		}
	} else {
		// No mBase and target is not aliased
		// Do nothing
		//LOGV(WVRRenderTP, "MakeAlias(%d, %u) noop", idx, resource);
	}
}

// Idx could be used by the target or the base.  It's depended on resource.
uint32_t FWaveVRTexturePool::MakeDepthAlias(int index, GLuint resource)
{
	LOG_FUNC();
	// We don't manage depth texture.
	if (!mBaseDepth.IsValid() || !mDepthPool[index].IsValid())
		return 0;

	check(mBaseDepth.IsValid());
	check(!mDepthPool.IsEmpty());
	check(mDepthPool[index].IsValid());

	FOpenGLTexture* target = GetOpenGLTextureFromRHITexture(mDepthPool[index]);
	FOpenGLTexture* base = mBaseDepth.IsValid() ? GetOpenGLTextureFromRHITexture(mBaseDepth) : target;  // this will always use mBaseDepth
	// mBaseColor of course is aliased.  target could be aliased or not.
	if (mBaseDepth.IsValid())
	{
		if (resource == 0) {
			// Has mBase and use idx target.
			// Alias the the target resource from texture pool
			//GLRHI->RHIAliasTextureResources
			base->AliasResources(*target);
			//LOGV(WVRRenderTP, "MakeDepthAlias(%d, 0) -> target %u", index, target->Resource);
			return target->GetResource();
		} else {
			// For using unreal texture queue, the resource not match the wvr queue's idx.
			if (base->GetResource() != resource) {
				//LOGV(WVRRenderTP, "MakeDepthAlias(%d, %u) -> originalResource %u -> %u", index, resource, base->Resource, resource);
				base->SetResource(resource);
				return resource;
			} else {
				//LOGV(WVRRenderTP, "MakeDepthAlias(%d, %u) noop", index, resource);
				return resource;
			}
		}
	} else {
		// No mBase and target is not aliased
		// Do nothing
		//LOGV(WVRRenderTP, "MakeDepthAlias(%d, %u) noop", index, resource);
		return resource;
	}
}

uint32 FWaveVRTexturePool::FindIndexByGLId(GLuint id) const
{
	check(id);

	// Find the index
	for (int i = 0; i < textureInfo.capacity; i++) {
		if (NATIVE_RESOURCE_TO_INT(mColorPool[i]->GetNativeResource()) == id) {
			//LOGD(WVRRenderTP, "FindIndexByGLId(id=%u and index is %d)", id, i);
			return i;
		}
	}

	// Imposible
	LOGD(WVRRenderTP, "FindIndexByGLId(id=%u) texture was not found in pool", id);
	for (int i = 0; i < textureInfo.capacity; i++) {
		LOGD(WVRRenderTP, "  mColorPool[%d]=%u", i, NATIVE_RESOURCE_TO_INT(mColorPool[i]->GetNativeResource()));
	}
	check(false);
	return 0;
}


uint32 FWaveVRTexturePool::FindIndexByTexture(FOpenGLTexture* OpenGLTexture) const
{
	check(OpenGLTexture);

	// Find the index
	for (int i = 0; i < textureInfo.capacity; i++) {
		check(mColorPool[i].IsValid());
		if (GetOpenGLTextureFromRHITexture(mColorPool[i]) == OpenGLTexture) {
			return i;
		}
	}
	// Imposible
	check(false);
	return 0;
}

const TRefCountPtr<FOpenGLTexture> FWaveVRTexturePool::GetGLTexture(uint32 index) const
{
	if (mBaseColor.IsValid()) {
		return GetOpenGLTextureFromRHITexture(mBaseColor);
	} else {
		check(index < textureInfo.capacity);
		check(mColorPool[index].IsValid());
		return GetOpenGLTextureFromRHITexture(mColorPool[index]);
	}
}

FTextureRHIRef FWaveVRTexturePool::GetRHIColor(uint32 index) const
{
	LOG_FUNC();
	if (mBaseColor.IsValid()) {
		return mBaseColor;
	} else {
		check(index < textureInfo.capacity);
		check(mColorPool[index].IsValid());
		return mColorPool[index];
	}
}

// Because the Engine will always get depth index 0.  The real index is maintained by color side.
FTextureRHIRef FWaveVRTexturePool::GetRHIDepth(uint32 index) const
{
	LOG_FUNC();
	return mBaseDepth;
}

void FWaveVRTexturePool::ReleaseTextures()
{
	bool hasBaseTexture = !!mBaseColor;
	LOG_FUNC();
	if (!mColorPool.IsEmpty()) {
		for (int i = 0; i < textureInfo.capacity; i++) {
			LOGD(WVRRenderTP, "TexturePool: Release color texture %d", i);
			// When Refs count to zero, the texture's destructor will call Release() itself.  We don't have to call Release().
			//if (hasBaseTexture && mColorPool[i])
			//	mColorPool[i]->Release();
			mColorPool[i].SafeRelease();
		}
	}

	if (!mDepthPool.IsEmpty()) {
		for (int i = 0; i < textureInfo.capacity; i++) {
			LOGD(WVRRenderTP, "TexturePool: Release depth texture %d", i);
			// When Refs count to zero, the texture's destructor will call Release() itself.  We don't have to call Release().
			//if (hasBaseTexture && mColorPool[i])
			//	mColorPool[i]->Release();
			mDepthPool[i].SafeRelease();
		}
	}

	if (mBaseColor.IsValid()) {
		LOGD(WVRRenderTP, "TexturePool: Release Base color");
		mBaseColor.SafeRelease();
	}

	if (mBaseDepth.IsValid()) {
		LOGD(WVRRenderTP, "TexturePool: Release Base depth");
		mBaseDepth.SafeRelease();
	}

	if (textureInfo.wvrTextureQueue) {
		LOGD(WVRRenderTP, "TexturePool: Release wvrTextureQueue");
		WVR()->ReleaseTextureQueue(textureInfo.wvrTextureQueue);
		textureInfo.wvrTextureQueue = nullptr;
	}

	LOGD(WVRRenderTP, "TexturePool: Released");
}

}  // namespace Render
}  // namespace Wave
