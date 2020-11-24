// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include <wrl/client.h>
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

using namespace Microsoft::WRL;

class FD3D12CommandContext;

#include "SharedTextureReceiver.generated.h"

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
struct ID3D12Fence;

struct ID3D11Device;
struct ID3D11Texture2D;
struct ID3D11DeviceContext;
class UTexture2D;

UENUM(BlueprintType)
enum class ESharedPixelFormat : uint8
{
	SPF_Unknown = 0,
	SPF_A32B32G32R32F = 1,
	SPF_B8G8R8A8 = 2,
	SPF_G8 = 3,
	SPF_G16 = 4,
	SPF_DXT1 = 5,
	SPF_DXT3 = 6,
	SPF_DXT5 = 7,
	SPF_UYVY = 8,
	SPF_FloatRGB = 9,
	SPF_FloatRGBA = 10,
	SPF_DepthStencil = 11,
	SPF_ShadowDepth = 12,
	SPF_R32_FLOAT = 13,
	SPF_G16R16 = 14,
	SPF_G16R16F = 15,
	SPF_G16R16F_FILTER = 16,
	SPF_G32R32F = 17,
	SPF_A2B10G10R10 = 18,
	SPF_A16B16G16R16 = 19,
	SPF_D24 = 20,
	SPF_R16F = 21,
	SPF_R16F_FILTER = 22,
	SPF_BC5 = 23,
	SPF_V8U8 = 24,
	SPF_A1 = 25,
	SPF_FloatR11G11B10 = 26,
	SPF_A8 = 27,
	SPF_R32_UINT = 28,
	SPF_R32_SINT = 29,
	SPF_PVRTC2 = 30,
	SPF_PVRTC4 = 31,
	SPF_R16_UINT = 32,
	SPF_R16_SINT = 33,
	SPF_R16G16B16A16_UINT = 34,
	SPF_R16G16B16A16_SINT = 35,
	SPF_R5G6B5_UNORM = 36,
	SPF_R8G8B8A8 = 37,
	SPF_A8R8G8B8 = 38,	// Only used for legacy loading; do NOT use!
	SPF_BC4 = 39,
	SPF_R8G8 = 40,
	SPF_ATC_RGB = 41,
	SPF_ATC_RGBA_E = 42,
	SPF_ATC_RGBA_I = 43,
	SPF_X24_G8 = 44,	// Used for creating SRVs to alias a DepthStencil buffer to read Stencil. Don't use for creating textures.
	SPF_ETC1 = 45,
	SPF_ETC2_RGB = 46,
	SPF_ETC2_RGBA = 47,
	SPF_R32G32B32A32_UINT = 48,
	SPF_R16G16_UINT = 49,
	SPF_ASTC_4x4 = 50,	// 8.00 bpp
	SPF_ASTC_6x6 = 51,	// 3.56 bpp
	SPF_ASTC_8x8 = 52,	// 2.00 bpp
	SPF_ASTC_10x10 = 53,	// 1.28 bpp
	SPF_ASTC_12x12 = 54,	// 0.89 bpp
	SPF_BC6H = 55,
	SPF_BC7 = 56,
	SPF_R8_UINT = 57,
	SPF_L8 = 58,
	SPF_XGXR8 = 59,
	SPF_R8G8B8A8_UINT = 60,
	SPF_R8G8B8A8_SNORM = 61,
	SPF_R16G16B16A16_UNORM = 62,
	SPF_R16G16B16A16_SNORM = 63,
	SPF_PLATFORM_HDR_0 = 64,	// Indicates platform-specific HDR output formats
	SPF_PLATFORM_HDR_1 = 65,	// Reserved.
	SPF_PLATFORM_HDR_2 = 66,	// Reserved.
	SPF_NV12 = 67,
	SPF_MAX = 68
};

UCLASS(BlueprintType)
class BLUEWINDOW_API USharedTextureReceiver : public UObject
{
	GENERATED_BODY()
private:
	//DX12 resources
    ID3D12Device* D3D12Device;
	ComPtr<ID3D12CommandQueue> D3D12CmdQueue;
	ComPtr<ID3D12CommandAllocator> D3D12CmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> D3D12GfxCmdList;
	ComPtr<ID3D12Resource> D3D12SharedTexture;

	//UINT m_frameIndex;
	HANDLE FenceEvent;
	uint64 FenceValue;
	ComPtr<ID3D12Fence> D3D12Fence;

	//DX11 resource
	ID3D11Device* D3D11Device;
	ID3D11DeviceContext* D3D11ImmediateContext;
	ComPtr<ID3D11Texture2D> D3D11SharedTexture;

	FString currRHI;

	UPROPERTY(Transient)
	UTexture2D* DstTexture;

	int64 Handle_;

	bool failure_;

public:

	//~USharedTextureReceiver();

	void Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format = ESharedPixelFormat::SPF_R8G8B8A8);
	void Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format = ESharedPixelFormat::SPF_R8G8B8A8);

	UFUNCTION(BlueprintCallable, Category = "SharedTexture")
	static USharedTextureReceiver* CreateSharedTextureReceiver(
		int Width, int Height,
		int64 Handle,
		ESharedPixelFormat Format = ESharedPixelFormat::SPF_R8G8B8A8
	);

	UFUNCTION(BlueprintCallable, Category = "SharedTexture")
	UTexture2D* GetTexture();

	UFUNCTION(BlueprintPure, Category = "SharedTexture")
	UTexture2D* GetTexturePure();
};