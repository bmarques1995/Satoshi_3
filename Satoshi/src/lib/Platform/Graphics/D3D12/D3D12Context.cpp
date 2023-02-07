#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D12/D3D12Context.hpp"
#include "Satoshi/Core/Application.hpp"
#include "Satoshi/Core/Console.hpp"

Satoshi::D3D12Context::D3D12Context(StWindowHandle window, uint32_t width, uint32_t height, uint32_t numBackBuffers, uint32_t numFramesInFlight) :
	m_VSync(false), m_BackBuffersAmount(numBackBuffers), m_FramesInFlightAmount(numFramesInFlight)
{
	m_ClearColor[0] = 232.0f/255.0f;
	m_ClearColor[1] = 177.0f / 255.0f;
	m_ClearColor[2] = .0f;
	m_ClearColor[3] = 1.0f;

	m_RenderTargetResource = new ID3D12Resource*[m_BackBuffersAmount];

	CreateDevice();
	CreateHeapDescriptor();
	CreateQueueDescriptor();
	CreateFrameContext();
	CreateCommandList();
	CreateFence();
	CreateSwapChain(window);
	CreateRenderTargetView();
	CreateViewport(width, height);
}

Satoshi::D3D12Context::~D3D12Context()
{
	CleanupRenderTargetView();
	delete[] m_FrameContext;
	delete[] m_RenderTargetDescriptor;
	delete[] m_RenderTargetResource;
}

void Satoshi::D3D12Context::ClearTarget()
{
	m_CommandList->ClearRenderTargetView(m_RenderTargetDescriptor[m_CurrentFrame.BackBufferIndex], m_ClearColor, 0, NULL);
	m_CommandList->OMSetRenderTargets(1, &m_RenderTargetDescriptor[m_CurrentFrame.BackBufferIndex], FALSE, NULL);
	m_CommandList->SetDescriptorHeaps(1, m_SourceDescHeap.GetAddressOf());
}

void Satoshi::D3D12Context::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}

void Satoshi::D3D12Context::ReceiveCommands()
{
	m_CurrentFrame.Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_CurrentFrame.Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_CurrentFrame.Barrier.Transition.pResource = m_RenderTargetResource[m_CurrentFrame.BackBufferIndex];
	m_CurrentFrame.Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_CurrentFrame.Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_CurrentFrame.Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CommandList->Reset(m_CurrentFrame.FrameContext->CommandAllocator.Get(), NULL);
	m_CommandList->ResourceBarrier(1, &m_CurrentFrame.Barrier);
}

void Satoshi::D3D12Context::DispatchCommands()
{
	m_CurrentFrame.Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CurrentFrame.Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CommandList->ResourceBarrier(1, &m_CurrentFrame.Barrier);
	m_CommandList->Close();
	m_CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_CommandList.GetAddressOf());
}

void Satoshi::D3D12Context::Draw(uint32_t elements)
{
}

void Satoshi::D3D12Context::NewFrame()
{
	m_CurrentFrame.NextFrameIndex = m_FrameIndex + 1;
	m_FrameIndex = m_CurrentFrame.NextFrameIndex;

	UpdateFrameContext(&m_CurrentFrame.FenceValue, &m_CurrentFrame.FrameContext, &m_CurrentFrame.BackBufferIndex, m_CurrentFrame.NextFrameIndex);
}

void Satoshi::D3D12Context::EndFrame()
{
	UpdateFence(&m_CurrentFrame.FrameContext, &m_CurrentFrame.FenceValue);
}

void Satoshi::D3D12Context::Present()
{
	m_SwapChain->Present(m_VSync ? 1 : 0, 0);
}

bool Satoshi::D3D12Context::IsVSync()
{
	return m_VSync;
}

void Satoshi::D3D12Context::SetVSync(bool isVSync)
{
	m_VSync = isVSync;
}

void Satoshi::D3D12Context::GetGPUName(std::string* output)
{
	auto adapterDescription = DXGI_ADAPTER_DESC();
	m_Adapter->GetDesc(&adapterDescription);
	std::wstring name = adapterDescription.Description;
	*output = std::string(name.begin(), name.end());
}

void Satoshi::D3D12Context::OnResize(WindowResizeEvent& e)
{
	WaitLastFrame();
	CleanupRenderTargetView();
	HRESULT result = m_SwapChain->ResizeBuffers(0, e.GetWidth(), e.GetHeight(), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
	Console::Log("Max swapchain {0}", DXGI_MAX_SWAP_CHAIN_BUFFERS);
	assert(SUCCEEDED(result) && "Failed to resize swapchain.");
	CreateRenderTargetView();
}

void Satoshi::D3D12Context::CreateDevice()
{
	HRESULT hr;

#if defined (_DEBUG) || defined(DEBUG)
	ID3D12Debug* m_Debugger;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debugger));
	assert(hr == S_OK);
	m_Debugger->EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;
	hr = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(m_Device.GetAddressOf()));
	assert(hr == S_OK);

#if defined (_DEBUG) || defined(DEBUG)

	ID3D12InfoQueue* infoQueue;
	D3D12_MESSAGE_ID hide[] =
	{
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		// Workarounds for debug layer issues on hybrid-graphics systems
		D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
	};
	D3D12_INFO_QUEUE_FILTER filter = {};
	filter.DenyList.NumIDs = static_cast<UINT>(sizeof(hide) / sizeof(D3D12_MESSAGE_ID));
	filter.DenyList.pIDList = hide;
	m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true);
	infoQueue->AddStorageFilterEntries(&filter);
	infoQueue->Release();
	m_Debugger->Release();

#endif
}

void Satoshi::D3D12Context::CreateHeapDescriptor()
{
	HRESULT hr;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = m_BackBuffersAmount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 1;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_RenderTargetViewDescHeap.GetAddressOf()));
	assert(hr == S_OK);

	size_t rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RenderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();
	m_RenderTargetDescriptor = new D3D12_CPU_DESCRIPTOR_HANDLE[m_BackBuffersAmount];
	for (uint32_t i = 0; i < m_BackBuffersAmount; i++)
	{
		m_RenderTargetDescriptor[i] = rtvHandle;
		rtvHandle.ptr += rtvDescriptorSize;
	}

	heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_SourceDescHeap.GetAddressOf()));
	assert(hr == S_OK);
}

void Satoshi::D3D12Context::CreateQueueDescriptor()
{
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()));
	assert(hr == S_OK);
}

void Satoshi::D3D12Context::CreateFrameContext()
{
	HRESULT hr;
	m_FrameContext = new FrameContext[m_FramesInFlightAmount];
	for (uint32_t i = 0; i < m_FramesInFlightAmount; i++)
	{
		m_FrameContext[i].FenceValue = 0;
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_FrameContext[i].CommandAllocator.GetAddressOf()));
		assert(hr == S_OK);
	}
}

void Satoshi::D3D12Context::CreateCommandList()
{
	HRESULT hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContext[0].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf()));
	assert(hr == S_OK);
	hr = m_CommandList->Close();
	assert(hr == S_OK);
}

void Satoshi::D3D12Context::CreateFence()
{
	HRESULT hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
	assert(hr == S_OK);

	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
	assert(m_FenceEvent != nullptr);
}

void Satoshi::D3D12Context::CreateSwapChain(HWND windowHandle)
{
	m_RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.BufferCount = m_BackBuffersAmount;
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;
	swapChainDesc.Format = m_RTVFormat;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.Stereo = FALSE;

	UINT dxgiFactoryFlags = 0;


	IDXGIFactory4* dxgiFactory = nullptr;
	IDXGISwapChain1* swapChainTemp = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	assert(hr == S_OK);
	hr = dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &swapChainTemp);
	assert(hr == S_OK);
	hr = swapChainTemp->QueryInterface(IID_PPV_ARGS(m_SwapChain.GetAddressOf()));
	assert(hr == S_OK);

	hr = dxgiFactory->EnumAdapters1(0, m_Adapter.GetAddressOf());
	assert(hr == S_OK);

	swapChainTemp->Release();
	dxgiFactory->Release();
	m_SwapChain->SetMaximumFrameLatency(m_BackBuffersAmount);
	m_SwapChainWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();
}

void Satoshi::D3D12Context::CreateRenderTargetView()
{
	

	for (uint32_t i = 0; i < m_BackBuffersAmount; i++)
	{
		ID3D12Resource* pBackBuffer = nullptr;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		m_Device->CreateRenderTargetView(pBackBuffer, nullptr, m_RenderTargetDescriptor[i]);
		m_RenderTargetResource[i] = pBackBuffer;
	}
}

void Satoshi::D3D12Context::CreateViewport(uint32_t width, uint32_t height)
{
	m_Viewport.TopLeftX = m_Viewport.TopLeftY = m_ScissorRect.left = m_ScissorRect.top = 0;
	m_Viewport.Width = m_ScissorRect.right = (float)width;
	m_Viewport.Height = m_ScissorRect.bottom = (float)height;
	m_Viewport.MinDepth = .0f;
	m_Viewport.MaxDepth = 1.0f;
}

void Satoshi::D3D12Context::UpdateFrameContext(uint64_t* fenceValue, FrameContext** frameContext, uint32_t* backBufferIndex, uint32_t nextFrameIndex)
{
	HANDLE waitableObjects[] = { m_SwapChainWaitableObject, nullptr };
	DWORD numWaitableObjects = 1;

	*frameContext = &m_FrameContext[nextFrameIndex % 3];
	*fenceValue = (*frameContext)->FenceValue;
	if (fenceValue != 0)
	{
		(*frameContext)->FenceValue = 0;
		m_Fence->SetEventOnCompletion(*fenceValue, m_FenceEvent);
		waitableObjects[1] = m_FenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	*backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	(*frameContext)->CommandAllocator->Reset();
}

void Satoshi::D3D12Context::UpdateFence(FrameContext** frameContext, uint64_t* fenceValue)
{
	*fenceValue = m_FenceLastSignaledValue + 1;
	m_CommandQueue->Signal(m_Fence.Get(), *fenceValue);
	m_FenceLastSignaledValue = *fenceValue;
	(*frameContext)->FenceValue = *fenceValue;
}

void Satoshi::D3D12Context::WaitLastFrame()
{
	FrameContext* frameCtx = &m_FrameContext[m_FrameIndex % m_FramesInFlightAmount];

	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if (m_Fence->GetCompletedValue() >= fenceValue)
		return;

	m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
	WaitForSingleObject(m_FenceEvent, INFINITE);
}

void Satoshi::D3D12Context::CleanupRenderTargetView()
{
	WaitLastFrame();

	for (UINT i = 0; i < m_BackBuffersAmount; i++)
		if (m_RenderTargetResource[i]) 
		{ 
			m_RenderTargetResource[i]->Release();
			m_RenderTargetResource[i] = nullptr;
		}
}

#endif // ST_PLATFORM_WINDOWS
