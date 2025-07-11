#pragma once
#include "OS-ImGui_Struct.h"
#include "OS-ImGui_Exception.hpp"
#include <iostream>
#include <string>
#include <functional>
#include <codecvt>
#include <vector>
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

/****************************************************
* Copyright (C)	: Liv
* @file			: OS-ImGui_Base.h
* @author		: Liv
* @email		: 1319923129@qq.com
* @version		: 1.1
* @date			: 2024/4/4 13:59
****************************************************/

namespace OSImGui
{
	class D3DDevice
	{
	public:
		ID3D11Device* g_pd3dDevice = nullptr;
		ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
		IDXGISwapChain* g_pSwapChain = nullptr;
		ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
	};

	static D3DDevice g_Device;

	enum WindowType
	{
		NEW,
		ATTACH,
		INTERNAL
	};

	enum DirectXType
	{
		DX9,
		DX11,
		AUTO
	};

#if _WIN64
	typedef DWORD64 Address;
#else
	typedef DWORD Address;
#endif

	class WindowData
	{
	public:
		HWND  hWnd = NULL;
		HINSTANCE hInstance = nullptr;
		std::string Name;
		std::wstring wName;
		std::string ClassName;
		std::wstring wClassName;
		Vec2 Pos;
		Vec2 Size;
		ImColor BgColor{ 255, 255, 255 };
		bool Transparent = false;
	};

	class OSImGui_Base
	{
	public:
		// �ص�����
		std::function<void()> CallBackFn = nullptr;
		// �˳���ʶ
		bool EndFlag = false;
	public:
		// ��������
		WindowData Window;
		// Ŀ�괰������
		WindowData DestWindow;
	public:
		// ����һ���´���
		virtual void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack) {};
		// �˳�
		virtual void Quit() { EndFlag = true; };
	public:
		virtual bool CreateMyWindow() = 0;
		virtual void MainLoop() {};
		bool InitImGui(ID3D11Device* device, ID3D11DeviceContext* device_context);
		void CleanImGui();
		static std::wstring StringToWstring(const std::string& str);
	};
}