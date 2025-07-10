#include "OS-ImGui_External.h"
#include "OS-ImGui_Base.h"
#include <dxgi.h>
#include <winerror.h>
/****************************************************
* Copyright (C)	: Liv
* @file			: OS-ImGui_External.cpp
* @author		: Liv
* @email		: 1319923129@qq.com
* @version		: 1.1
* @date			: 2024/4/4 14:12
****************************************************/

    #include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <dxgi1_3.h>


// D3D11 Device
namespace OSImGui
{
    bool D3DDevice::CreateDeviceD3D(HWND hWnd)
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        UINT createDeviceFlags = 0;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
        if (res == DXGI_ERROR_UNSUPPORTED)
            res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
        if (res != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void D3DDevice::CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    }

    void D3DDevice::CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer == nullptr)
            return;
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    void D3DDevice::CleanupRenderTarget()
    {
        if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    }
}

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// OSImGui External
namespace OSImGui
{
void DisableDWM() {
    HRESULT hr = DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
    if (FAILED(hr)) {
        // Handle error
    }
}
#ifndef OSIMGUI_INTERNAL
    LRESULT WINAPI WndProc_External(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void OSImGui_External::NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack, bool Transparent)
    {
        if (!CallBack)
            throw OSException("CallBack is empty");
        if (WindowName.empty())
            Window.Name = "Window";

        Window.Name = WindowName;
        Window.wName = StringToWstring(Window.Name);
        Window.ClassName = "WindowClass";
        Window.wClassName = StringToWstring(Window.ClassName);
        Window.Size = WindowSize;
        Window.Transparent = Transparent;

        if(Window.Transparent)
            Window.BgColor = ImColor(0, 0, 0, 0);

        Type = NEW;
        CallBackFn = CallBack;

        if (!CreateMyWindow())
            throw OSException("CreateMyWindow() call failed");

        try {
            InitImGui(g_Device.g_pd3dDevice, g_Device.g_pd3dDeviceContext);
        }
        catch (OSException& e)
        {
            throw e;
        }

        MainLoop();
    }

    void  OSImGui_External::AttachAnotherWindow(std::string DestWindowName, std::string DestWindowClassName, std::function<void()> CallBack)
    {
        if (!CallBack)
            throw OSException("CallBack is empty");
        if (DestWindowName.empty() && DestWindowClassName.empty())
            throw OSException("DestWindowName and DestWindowClassName are empty");

        Window.Name = "Window";
        Window.wName = StringToWstring(Window.Name);
        Window.ClassName = "WindowClass";
        Window.wClassName = StringToWstring(Window.ClassName);
        Window.BgColor = ImColor(0, 0, 0, 0);
        Window.Transparent = true;

        DestWindow.hWnd = FindWindowA(
            (DestWindowClassName.empty() ? NULL : DestWindowClassName.c_str()),
            (DestWindowName.empty() ? NULL : DestWindowName.c_str()));
        if (DestWindow.hWnd == NULL)
            throw OSException("DestWindow isn't exist");
        DestWindow.Name = DestWindowName;
        DestWindow.ClassName = DestWindowClassName;

        Type = ATTACH;
        CallBackFn = CallBack;

        if (!CreateMyWindow())
            throw OSException("CreateMyWindow() call failed");

        try {
            InitImGui(g_Device.g_pd3dDevice,g_Device.g_pd3dDeviceContext);
        }
        catch (OSException& e)
        {
            throw e;
        }

        MainLoop();
    }

    bool OSImGui_External::PeekEndMessage()
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                return true;
        }
        return false;
    }

bool DesktopCaptureAndDrawToImGui(ID3D11Device* device, ID3D11DeviceContext* context)
{
    static bool initialized = false;
    static IDXGIOutputDuplication* desktopDupl = nullptr;
    static ID3D11Texture2D* sharedTex = nullptr;
    static ID3D11ShaderResourceView* srv = nullptr;
    static D3D11_TEXTURE2D_DESC lastDesc = {};

    if (!initialized)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        if (FAILED(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice))))
            return false;

        ComPtr<IDXGIAdapter> adapter;
        if (FAILED(dxgiDevice->GetAdapter(&adapter)))
            return false;

        ComPtr<IDXGIOutput> output;
        if (FAILED(adapter->EnumOutputs(0, &output)))
            return false;

        ComPtr<IDXGIOutput1> output1;
        if (FAILED(output->QueryInterface(IID_PPV_ARGS(&output1))))
            return false;

        if (FAILED(output1->DuplicateOutput(device, &desktopDupl)))
            return false;

        initialized = true;
    }

    DXGI_OUTDUPL_FRAME_INFO frameInfo = {};
    ComPtr<IDXGIResource> desktopRes;

    HRESULT hr = desktopDupl->AcquireNextFrame(1, &frameInfo, &desktopRes);
    if (FAILED(hr))
    {
        if (srv)
        {
            ImGui::GetBackgroundDrawList()->AddImage(
                (void*)srv,
                ImVec2(0, 0),
                ImVec2((float)lastDesc.Width, (float)lastDesc.Height)
            );
            return true;
        }
        return false;
    }

    ComPtr<ID3D11Texture2D> tex;
    if (FAILED(desktopRes->QueryInterface(IID_PPV_ARGS(&tex))))
    {
        desktopDupl->ReleaseFrame();
        return false;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    tex->GetDesc(&desc);

    // Avoid unnecessary recreation
    bool needRecreate =
        !sharedTex ||
        desc.Width != lastDesc.Width ||
        desc.Height != lastDesc.Height ||
        desc.Format != lastDesc.Format;

    if (needRecreate)
    {
        if (sharedTex) sharedTex->Release();
        if (srv) srv->Release();

        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        if (FAILED(device->CreateTexture2D(&desc, nullptr, &sharedTex)))
        {
            desktopDupl->ReleaseFrame();
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        if (FAILED(device->CreateShaderResourceView(sharedTex, &srvDesc, &srv)))
        {
            sharedTex->Release();
            sharedTex = nullptr;
            desktopDupl->ReleaseFrame();
            return false;
        }

        lastDesc = desc;
    }

    context->CopyResource(sharedTex, tex.Get());
    desktopDupl->ReleaseFrame();

    ImGui::GetBackgroundDrawList()->AddImage(
        (void*)srv,
        ImVec2(0, 0),
        ImVec2((float)desc.Width, (float)desc.Height)
    );

    return true;
}



    void OSImGui_External::MainLoop()
    {
        SetWindowDisplayAffinity(Window.hWnd, WDA_EXCLUDEFROMCAPTURE);

        while (!EndFlag)
        {
            if (PeekEndMessage())
                break;
            if (Type == ATTACH)
            {
                if (!UpdateWindowData())
                    break;
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            DesktopCaptureAndDrawToImGui(g_Device.g_pd3dDevice, g_Device.g_pd3dDeviceContext);
            this->CallBackFn();

            ImGui::Render();
            const float clear_color_with_alpha[4] = { Window.BgColor.Value.x, Window.BgColor.Value.y , Window.BgColor.Value.z, Window.BgColor.Value.w };
            g_Device.g_pd3dDeviceContext->OMSetRenderTargets(1, &g_Device.g_mainRenderTargetView, NULL);
            g_Device.g_pd3dDeviceContext->ClearRenderTargetView(g_Device.g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_Device.g_pSwapChain->Present(this->vsync, 0); // Present with vs
        }
        CleanImGui();
    }

    bool OSImGui_External::CreateMyWindow()
    {
        WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc_External, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, Window.wClassName.c_str(), NULL };
        RegisterClassExW(&wc);
        if (Type == ATTACH)
        {
            Window.hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT, Window.wClassName.c_str(), Window.wName.c_str(), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, GetModuleHandle(NULL), NULL);
        }
        else
        {
            if(Window.Transparent)
                Window.hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT, Window.wClassName.c_str(), Window.wName.c_str(), WS_POPUP, (int)Window.Pos.x, (int)Window.Pos.y, (int)Window.Size.x, (int)Window.Size.y, NULL, NULL, wc.hInstance, NULL);
            else
                Window.hWnd = CreateWindowW(Window.wClassName.c_str(), Window.wName.c_str(), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, (int)Window.Pos.x, (int)Window.Pos.y, (int)Window.Size.x, (int)Window.Size.y, NULL, NULL, wc.hInstance, NULL);
        }

        if(Window.Transparent)
            SetLayeredWindowAttributes(Window.hWnd, 0, 255, LWA_ALPHA);

        Window.hInstance = wc.hInstance;

        if (!g_Device.CreateDeviceD3D(Window.hWnd))
        {
            g_Device.CleanupDeviceD3D();
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return false;
        }

        ShowWindow(Window.hWnd, SW_SHOWDEFAULT);
        UpdateWindow(Window.hWnd);

        return Window.hWnd != NULL;
    }

    bool OSImGui_External::UpdateWindowData()
    {
        POINT Point{};
        RECT Rect{};

        DestWindow.hWnd = FindWindowA(
            (DestWindow.ClassName.empty() ? NULL : DestWindow.ClassName.c_str()),
            (DestWindow.Name.empty() ? NULL : DestWindow.Name.c_str()));
        if (DestWindow.hWnd == NULL)
            return false;

        GetClientRect(DestWindow.hWnd, &Rect);
        ClientToScreen(DestWindow.hWnd, &Point);

        Window.Pos = DestWindow.Pos = Vec2(static_cast<float>(Point.x), static_cast<float>(Point.y));
        Window.Size = DestWindow.Size = Vec2(static_cast<float>(Rect.right), static_cast<float>(Rect.bottom));

        SetWindowPos(Window.hWnd, HWND_TOPMOST, (int)Window.Pos.x, (int)Window.Pos.y, (int)Window.Size.x, (int)Window.Size.y, SWP_SHOWWINDOW);

        // ���ƴ���״̬�л�
        POINT MousePos;
        GetCursorPos(&MousePos);
        ScreenToClient(Window.hWnd, &MousePos);
        ImGui::GetIO().MousePos.x = static_cast<float>(MousePos.x);
        ImGui::GetIO().MousePos.y = static_cast<float>(MousePos.y);

        if (ImGui::GetIO().WantCaptureMouse)
            SetWindowLong(Window.hWnd, GWL_EXSTYLE, GetWindowLong(Window.hWnd, GWL_EXSTYLE) & (~WS_EX_LAYERED));
        else
            SetWindowLong(Window.hWnd, GWL_EXSTYLE, GetWindowLong(Window.hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        return true;
    }

    LRESULT WINAPI WndProc_External(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_CREATE:
        {
            MARGINS     Margin = { -1 };
            DwmExtendFrameIntoClientArea(hWnd, &Margin);
            break;
        }
        case WM_SIZE:
            if (g_Device.g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                g_Device.CleanupRenderTarget();
                g_Device.g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                g_Device.CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }

#endif
}