
#include "SharedTextureReceiver.h"

#define USE_RHI_GFXCMDLIST 0
#define BLOCK_RENDER_THREAD_UNTIL_COPY_IS_COMPLETE 1

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

#define RECEIVER_FAIL(m) UE_LOG(LogTemp, Error, TEXT(m)); failure_ = true; return
#define HRESULT_FAIL(hrs, m) if(FAILED(hrs)) { UE_LOG(LogTemp, Error, TEXT(m)); UE_LOG(LogTemp, Error, TEXT("HRESULT is %s"), hrs); failure_ = true; return; }

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

#if !USE_RHI_GFXCMDLIST
		auto hres = D3D12Device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COPY,
			IID_PPV_ARGS(&D3D12CmdAllocator)
		);
		HRESULT_FAIL(hres, "Failed to create command allocator for receiving shared textures");

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

		hres = D3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&D3D12CmdQueue));
		HRESULT_FAIL(hres, "Failed to create command queue for receiving shared textures");

		hres = D3D12Device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_COPY,
			D3D12CmdAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&D3D12GfxCmdList)
		);
		HRESULT_FAIL(hres, "Failed to create graphics command list for receiving shared textures");

		hres = D3D12GfxCmdList->Close();
		HRESULT_FAIL(hres, "Failed to create graphics command list for receiving shared textures");

		hres = D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D12Fence));
		HRESULT_FAIL(hres, "Failed to create internal fence for receiving shared textures");

		FenceEvent = CreateEvent(nullptr, false, false, nullptr);
#endif

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
		D3D11SharedTexture.Reset();
		D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
		D3D11Device->GetImmediateContext(&D3D11ImmediateContext);

		HRESULT openResult = D3D11Device->OpenSharedResource(
			reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
			IID_PPV_ARGS(&D3D11SharedTexture)
		);

		HRESULT_FAIL(openResult, "Failed to open shared resource");
	}
	if (currRHI.Equals("D3D12"))
	{
		D3D12SharedTexture.Reset();
		HRESULT openResult = D3D12Device->OpenSharedHandle(
			reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
			IID_PPV_ARGS(&D3D12SharedTexture)
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
				D3D11SharedTexture.Get()
			);
		});
	}
	else if (currRHI.Equals("D3D12"))
	{	
		ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
		{
			auto dstRes = static_cast<FTexture2DResource*>(DstTexture->Resource);
			auto dstResNative = static_cast<ID3D12Resource*>(dstRes->GetTexture2DRHI()->GetNativeResource());

#if USE_RHI_GFXCMDLIST
			FD3D12CommandContext& CmdCtx =
			    static_cast<FD3D12CommandContext&>(RHICmdList.GetContext());

			ID3D12GraphicsCommandList* GfxCmdList = CmdCtx.CommandListHandle.GraphicsCommandList();
			GfxCmdList->CopyResource(dstResNative, D3D12SharedTexture);
#else
			uint64 tempFence = FenceValue;
#if !BLOCK_RENDER_THREAD_UNTIL_COPY_IS_COMPLETE
			if(D3D12Fence->GetCompletedValue() < tempFence) return;
#endif

			D3D12CmdAllocator->Reset();
			D3D12GfxCmdList->Reset(D3D12CmdAllocator.Get(), nullptr);

            D3D12GfxCmdList->CopyResource(dstResNative, D3D12SharedTexture.Get());
			D3D12GfxCmdList->Close();

			ID3D12CommandList* ppCommandLists[] = { D3D12GfxCmdList.Get() };
			D3D12CmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			D3D12CmdQueue->Signal(D3D12Fence.Get(), tempFence);
			FenceValue++;

#if BLOCK_RENDER_THREAD_UNTIL_COPY_IS_COMPLETE
			if(D3D12Fence->GetCompletedValue() < tempFence)
			{
			    D3D12Fence->SetEventOnCompletion(tempFence, FenceEvent);
				WaitForSingleObject(FenceEvent, INFINITE);
			}
#endif
#endif
		});
	}
	return DstTexture;
}

UTexture2D* USharedTextureReceiver::GetTexturePure()
{
	return DstTexture;
}
