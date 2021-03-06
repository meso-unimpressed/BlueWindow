
#include "SharedTextureReceiver.h"
#include "HardwareInfo.h"
//#include "Engine/Runtime/Windows/Private/D3D11RHIPrivate.h"

#define DISPOSE_RESOURCE(resource) if(resource) { resource->Release(); delete resource; resource = nullptr; }
#define RECEIVER_FAIL(m) UE_LOG(LogTemp, Error, TEXT(m)); failuer_ = true; return
#define HRESULT_FAIL(hrs, m) if(FAILED(openResult)) { UE_LOG(LogTemp, Error, TEXT(m)); UE_LOG(LogTemp, Error, TEXT("HRESULT is %s"), hrs); failuer_ = true; return; }

//USharedTextureReceiver::~USharedTextureReceiver()
//{
//	DISPOSE_RESOURCE(sharedTexture);
//	//DISPOSE_RESOURCE(sharedResourceView);
//}

void USharedTextureReceiver::Initialize(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	failuer_ = false;
	currRHI = FHardwareInfo::GetHardwareInfo(NAME_RHI);
	if(currRHI.Equals("D3D11"))
	{
		D3D11Device = (ID3D11Device*)GDynamicRHI->RHIGetNativeDevice();
		D3D11Device->GetImmediateContext(&pImmediateContext);

		Update(Width, Height, Handle, Format);
	}
}

void USharedTextureReceiver::Update(int Width, int Height, int64 Handle, ESharedPixelFormat Format)
{
	failuer_ = false;
	DstTexture = UTexture2D::CreateTransient(Width, Height, (EPixelFormat)Format);
	DstTexture->UpdateResource();

	if (currRHI.Equals("D3D11"))
	{
		DISPOSE_RESOURCE(sharedTexture);
		//DISPOSE_RESOURCE(sharedResourceView);

		ID3D11Resource* sharedResource;

		HRESULT openResult = D3D11Device->OpenSharedResource(
			(HANDLE)uint64(Handle),
			__uuidof(ID3D11Resource),
			(void**)(&sharedResource)
		);

		HRESULT_FAIL(openResult, "Failed to open shared resource");

		sharedTexture = (ID3D11Texture2D*)sharedResource;

		//HRESULT createSrvResult = D3D11Device->CreateShaderResourceView(
		//	sharedResource,
		//	NULL,
		//	&sharedResourceView
		//);

		//HRESULT_FAIL(createSrvResult, "Failed to create SRV");

		//FD3D11DynamicRHI* DynamicRHI = static_cast<FD3D11DynamicRHI*>(GDynamicRHI);

		//GDynamicRHI->RHICreateTexture2DFromResource()
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
	if (failuer_)
	{
		return DstTexture;
	}

	if (currRHI.Equals("D3D11"))
	{
		ENQUEUE_RENDER_COMMAND(void)([this](FRHICommandListImmediate& RHICmdList)
		{
			auto dstRes = (FTexture2DResource*)DstTexture->Resource;
			pImmediateContext->CopyResource(
				(ID3D11Texture2D*)dstRes->GetTexture2DRHI()->GetNativeResource(),
				sharedTexture
			);
		});
	}

	return DstTexture;
}

UTexture2D* USharedTextureReceiver::GetTexturePure()
{
	return DstTexture;
}
