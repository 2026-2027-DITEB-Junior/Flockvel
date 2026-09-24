
#ifndef WINDOW_H_
#define WINDOW_H_

#include "../Engine/IBase.h"
#include "../Engine/Singleton.h"
#include <SDL3/SDL_video.h>
#include <string>

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace clc
{
	class API Window : public IBase
	{
		CLC_SINGLETON(Window);

	public:

		// Initialize the window (actually creates it)
		bool Initialize();

		// getters
		void* GetHandle();
		bool Exists();
		int GetPosX();
		int GetPosY();
		int GetWidth();
		int GetHeight();
		bool GetFullscreen();
		std::string GetTitle();
		void GetCurrentMonitorRect(int& left, int& top, int& right, int& bottom);   // Current monitor

		// setters
		void SetPosition(int x, int y);
		void SetPosX(int x);
		void SetPosY(int y);
		void SetSize(int w, int h);
		void SetWidth(int w);
		void SetHeight(int h);
		void SetTitle(const char* title);
		void SetFullscreen(bool enabled);
		void Close();

		// update to read all messages
		void Update();

	private:
		std::string m_Title;
		long m_WindowStyle = 0;
		int m_PosX = SDL_WINDOWPOS_CENTERED, m_PosY = SDL_WINDOWPOS_CENTERED;
		int m_Width = 1280, m_Height = 800;               // client area size of the window
		int m_MetricWidth = 0, m_MetricHeight = 0;   // metric size of the window
		bool m_Fullscreen = false;
		bool m_Exists = false;


		SDL_Window* m_window = nullptr;
		SDL_GLContext m_glContext = nullptr;
		bool m_initialized = false;
		bool m_running = false;
		float m_value = 0.0f;
		int m_counter = 0;
	};
}
#define clcWindow (clc::Window::GetInstance())
// ----------------------------------------------------------------------------
#endif
