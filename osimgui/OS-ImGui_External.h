#pragma once
#include "OS-ImGui_Base.h"

/****************************************************
* Copyright (C)	: Liv
* @file			: OS-ImGui_External.h
* @author		: Liv
* @email		: 1319923129@qq.com
* @version		: 1.1
* @date			: 2024/4/4 14:12
****************************************************/

namespace OSImGui
{
#ifndef OSIMGUI_INTERNAL

	class OSImGui_External : public OSImGui_Base
	{
	private:
		// ��������
		WindowType Type = NEW;
	public:
		// ����һ���´���
		bool vsync = true;
		void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack, bool Transparent = false);
		// ���ӵ���һ��������
		void AttachAnotherWindow(std::string DestWindowName, std::string DestWindowClassName, std::function<void()> CallBack);
	private:
		void MainLoop();
		bool UpdateWindowData();
		bool CreateMyWindow();
		bool PeekEndMessage();
	};

#endif
}