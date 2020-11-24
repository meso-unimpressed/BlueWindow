
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
#include <wrl/client.h>
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include "HardwareInfo.h"

using namespace Microsoft::WRL;

#define RECEIVER_FAIL(m) UE_LOG(LogTemp, Error, TEXT(m)); failure_ = true; return
#define HRESULT_FAIL(hrs, m) if(FAILED(hrs)) { UE_LOG(LogTemp, Error, TEXT(m)); UE_LOG(LogTemp, Error, TEXT("HRESULT is %s"), hrs); Failure = true; return; }

class FD3D11SharedTextureDetail : public FSharedTextureDetail
{
private:
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* ImmediateContext = nullptr;
	ComPtr<ID3D11Texture2D> SharedTexture;

public:
    virtual void Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format) override;
    virtual void Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format) override;
    virtual void Render(UTexture2D* DstTexture) override;
};

void FD3D11SharedTextureDetail::Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	Failure = false;
	Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	Device->GetImmediateContext(&ImmediateContext);

	Update(Width, Height, Handle, Format);
}

void FD3D11SharedTextureDetail::Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	Failure = false;
	SharedTexture.Reset();
	Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	Device->GetImmediateContext(&ImmediateContext);

	HRESULT openResult = Device->OpenSharedResource(
		reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
		IID_PPV_ARGS(&SharedTexture)
	);

	HRESULT_FAIL(openResult, "Failed to open shared resource");
}

void FD3D11SharedTextureDetail::Render(UTexture2D* DstTexture)
{
	if (Failure) return;
	ENQUEUE_RENDER_COMMAND(void)([this, DstTexture](FRHICommandListImmediate& RHICmdList)
		{
			auto dstRes = static_cast<FTexture2DResource*>(DstTexture->Resource);
			ImmediateContext->CopyResource(
				static_cast<ID3D11Texture2D*>(dstRes->GetTexture2DRHI()->GetNativeResource()),
				SharedTexture.Get()
			);
		});
}

class FD3D12SharedTextureDetail : public FSharedTextureDetail
{
private:
	ID3D12Device* Device = nullptr;
	ComPtr<ID3D12CommandQueue> CmdQueue;
	ComPtr<ID3D12CommandAllocator> CmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> GfxCmdList;
	ComPtr<ID3D12Resource> SharedTexture;

	HANDLE FenceEvent = nullptr;
	uint64 FenceValue = 0;
	ComPtr<ID3D12Fence> D3D12Fence;

public:
	virtual void Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format) override;
	virtual void Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format) override;
	virtual void Render(UTexture2D* DstTexture) override;
};

void FD3D12SharedTextureDetail::Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	Device = static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());

#if !USE_RHI_GFXCMDLIST
	auto hres = Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_COPY,
		IID_PPV_ARGS(&CmdAllocator)
	);
	HRESULT_FAIL(hres, "Failed to create command allocator for receiving shared textures");

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	hres = Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CmdQueue));
	HRESULT_FAIL(hres, "Failed to create command queue for receiving shared textures");

	hres = Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_COPY,
		CmdAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&GfxCmdList)
	);
	HRESULT_FAIL(hres, "Failed to create graphics command list for receiving shared textures");

	hres = GfxCmdList->Close();
	HRESULT_FAIL(hres, "Failed to create graphics command list for receiving shared textures");

	hres = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3D12Fence));
	HRESULT_FAIL(hres, "Failed to create internal fence for receiving shared textures");

	FenceEvent = CreateEvent(nullptr, false, false, nullptr);
#endif

	Update(Width, Height, Handle, Format);
}

void FD3D12SharedTextureDetail::Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	Failure = false;
	SharedTexture.Reset();
	auto hres = Device->OpenSharedHandle(
		reinterpret_cast<HANDLE>(static_cast<uint64>(Handle)),
		IID_PPV_ARGS(&SharedTexture)
	);

	HRESULT_FAIL(hres, "Failed to open shared resource");
}

void FD3D12SharedTextureDetail::Render(UTexture2D* DstTexture)
{
	if (Failure) return;
	ENQUEUE_RENDER_COMMAND(void)([this, DstTexture](FRHICommandListImmediate& RHICmdList)
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
		if (D3D12Fence->GetCompletedValue() < tempFence) return;
#endif

		CmdAllocator->Reset();
		GfxCmdList->Reset(CmdAllocator.Get(), nullptr);

		GfxCmdList->CopyResource(dstResNative, SharedTexture.Get());
		GfxCmdList->Close();

		ID3D12CommandList* ppCommandLists[] = { GfxCmdList.Get() };
		CmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		CmdQueue->Signal(D3D12Fence.Get(), tempFence);
		FenceValue++;

#if BLOCK_RENDER_THREAD_UNTIL_COPY_IS_COMPLETE
		if (D3D12Fence->GetCompletedValue() < tempFence)
		{
			D3D12Fence->SetEventOnCompletion(tempFence, FenceEvent);
			WaitForSingleObject(FenceEvent, INFINITE);
		}
#endif
#endif
	});
}

void USharedTextureReceiver::Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	CurrentRHI = FHardwareInfo::GetHardwareInfo(NAME_RHI);
	if (CurrentRHI.Equals("D3D11"))
	{
		Detail = MakeShared<FD3D11SharedTextureDetail>();
		Detail->Initialize(Width, Height, Handle, Format);
	}
	if (CurrentRHI.Equals("D3D12"))
	{
		Detail = MakeShared<FD3D12SharedTextureDetail>();
		Detail->Initialize(Width, Height, Handle, Format);
	}
}

void USharedTextureReceiver::Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	DstTexture = UTexture2D::CreateTransient(
	    Width, Height,
		static_cast<EPixelFormat>(Format)
	);
	DstTexture->UpdateResource();

	Detail->Update(Width, Height, Handle, Format);
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
	if (Detail->Failure) return DstTexture;
	Detail->Render(DstTexture);
	return DstTexture;
}

UTexture2D* USharedTextureReceiver::GetTexturePure()
{
	return DstTexture;
}
