
#include "SharedTextureReceiver.h"

#define USE_RHI_GFXCMDLIST 0

#if USE_RHI_GFXCMDLIST
#include "D3D12RHI/Private/D3D12CommandContext.h"
#endif

#include "Engine/Texture2D.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_2.h>
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include "HardwareInfo.h"

#define DISPOSE_RESOURCE(resource) if(resource) { resource->Release(); delete resource; resource = nullptr; }
#define RECEIVER_FAIL(m) UE_LOG(LogTemp, Error, TEXT(m)); failure_ = true; return
#define HRESULT_FAIL(hrs, m) if(FAILED(hrs)) { UE_LOG(LogTemp, Error, TEXT(m)); UE_LOG(LogTemp, Error, TEXT("HRESULT is %s"), hrs); failure_ = true; return; }

void USharedTextureReceiver::CreateD3D12CommandList()
{
	auto hres = D3D12Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_COPY,
		__uuidof(ID3D12CommandAllocator),
		reinterpret_cast<void**>(&D3D12CommandAllocator)
	);
	HRESULT_FAIL(hres, "Failed to create command allocator for receiving shared textures");

	hres = D3D12Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_COPY,
		D3D12CommandAllocator,
		nullptr,
		__uuidof(ID3D12GraphicsCommandList),
		reinterpret_cast<void**>(&D3D12GraphicsCommandList)
	);
	HRESULT_FAIL(hres, "Failed to create graphics command list for receiving shared textures");
}

void USharedTextureReceiver::Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	failure_ = false;
	currRHI = FHardwareInfo::GetHardwareInfo(NAME_RHI);
	if (currRHI.Equals("D3D11"))
	{
		D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
		D3D11Device->GetImmediateContext(&D3D11ImmediateContext);

		Update(Width, Height, Handle, Format);
	}
	if (currRHI.Equals("D3D12"))
	{
		D3D12Device = static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());

		Update(Width, Height, Handle, Format);
	}
}

void USharedTextureReceiver::Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	failure_ = false;
	DstTexture = UTexture2D::CreateTransient(
	    Width, Height,
		static_cast<EPixelFormat>(Format)
	);
	DstTexture->UpdateResource();

	if (currRHI.Equals("D3D11"))
	{
		DISPOSE_RESOURCE(D3D11SharedTexture);
		D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
		D3D11Device->GetImmediateContext(&D3D11ImmediateContext);

		ID3D11Resource* sharedResource;
		HRESULT openResult = D3D11Device->OpenSharedResource(
			reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
			__uuidof(ID3D11Resource),
			reinterpret_cast<void**>(&sharedResource)
		);

		D3D11SharedTexture = static_cast<ID3D11Texture2D*>(sharedResource);
	}
	if (currRHI.Equals("D3D12"))
	{
		DISPOSE_RESOURCE(D3D12SharedTexture);
		HRESULT openResult = D3D12Device->OpenSharedHandle(
			reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
			__uuidof(ID3D12Resource),
			reinterpret_cast<void**>(&D3D12SharedTexture)
		);

		HRESULT_FAIL(openResult, "Failed to open shared resource");
	}
}

USharedTextureReceiver* USharedTextureReceiver::CreateSharedTextureReceiver(
	int Width, int Height,
	int64 Handle,
	ESharedPixelFormat Format /*= ESharedPixelFormat::PF_R8G8B8A8 */
)
{
	USharedTextureReceiver* res = NewObject<USharedTextureReceiver>();
	res->Initialize(Width, Height, Handle, Format);
	return res;
}

UTexture2D* USharedTextureReceiver::GetTexture()
{
	if (failure_)
	{
		return DstTexture;
	}

	if (currRHI.Equals("D3D11"))
	{
		ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
		{
			auto dstRes = static_cast<FTexture2DResource*>(DstTexture->Resource);
			D3D11ImmediateContext->CopyResource(
				static_cast<ID3D11Texture2D*>(dstRes->GetTexture2DRHI()->GetNativeResource()),
				D3D11SharedTexture
			);
		});
	}
	else if (currRHI.Equals("D3D12"))
	{	
		ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
		{
			auto dstRes = static_cast<FTexture2DResource*>(DstTexture->Resource);

#if USE_RHI_GFXCMDLIST
			FD3D12CommandContext& CmdCtx =
			    static_cast<FD3D12CommandContext&>(RHICmdList.GetContext());

			ID3D12GraphicsCommandList* GfxCmdList = CmdCtx.CommandListHandle.GraphicsCommandList();
			GfxCmdList->CopyResource(
                static_cast<ID3D12Resource*>(dstRes->GetTexture2DRHI()->GetNativeResource()),
				D3D12SharedTexture
			);
#endif
		});
	}

	return DstTexture;
}

UTexture2D* USharedTextureReceiver::GetTexturePure()
{
	return DstTexture;
}
