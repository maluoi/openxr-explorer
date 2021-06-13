#include "imgui_skg.h"
#include "imgui_impl_skg.h"
#include "imgui_impl_win32.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "sk_gpu.h"
#define SOKOL_TIME_IMPL
#include "sokol_time.h"

skg_swapchain_t sk_swapchain = {};
int32_t         sk_width     = 1280;
int32_t         sk_height    = 800;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int arg_count, const char **args) {
	if (!app_args(arg_count, args))
		return 0;

//int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui sk_gpu shell"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, app_name, WS_OVERLAPPEDWINDOW, 100, 100, sk_width, sk_height, NULL, NULL, wc.hInstance, NULL);

	stm_setup();

	// Initialize Direct3D
	skg_callback_log([](skg_log_ level, const char *text) { 
		if (level != skg_log_info)
			printf("[%d] %s\n", level, text); 
		});
	if (!skg_init(app_name, nullptr)) {
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	sk_swapchain = skg_swapchain_create(hwnd, skg_tex_fmt_rgba32_linear, skg_tex_fmt_depth16, 1280, 800);

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	/*ImFontConfig config = ImFontConfig();
	config.OversampleV = 1;
	config.OversampleH = 1;
	ImFont* font = io.Fonts->AddFontFromFileTTF("CascadiaMono.ttf", 18.0f, &config);
	IM_ASSERT(font != NULL);*/

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	if (!app_init())
		return 2;

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplSkg_Init();

	// Our state
	bool   show_demo_window = false;
	ImVec4 clear_color      = ImVec4(0, 0, 0, 1.00f); //ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	bool    evts             = true;
	bool    run              = true;
	int64_t last_update_time = 0;
	while (run) {
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			evts = true;
			if (msg.message == WM_QUIT) run = false;
		}
		if (!evts && stm_sec(stm_since(last_update_time)) > 0.4f) evts = true;
		if (!evts) { Sleep(3); continue; }
		evts = false;
		last_update_time = stm_now();

		// Start the Dear ImGui frame
		skg_draw_begin();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		app_step({(float)sk_width, (float)sk_height});

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// Rendering
		ImGui::Render();
		skg_swapchain_bind(&sk_swapchain);
		skg_target_clear(true, (float *)&clear_color);
		ImGui_ImplSkg_RenderDrawData(ImGui::GetDrawData());

		skg_swapchain_present(&sk_swapchain);
	}

	app_shutdown();

	// Cleanup
	ImGui_ImplSkg_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	skg_swapchain_destroy(&sk_swapchain);
	skg_shutdown();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			sk_width  = LOWORD(lParam);
			sk_height = HIWORD(lParam);
			skg_swapchain_resize(&sk_swapchain, (UINT)sk_width, (UINT)sk_height);
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
