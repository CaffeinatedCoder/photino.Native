#ifndef Photino_H
#define Photino_H

#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>
#include <wrl.h>
#include <map>
#include <string>
#include <wil/com.h>
#include <WebView2.h>
typedef const wchar_t* AutoString;
#else
#ifdef OS_LINUX
#include <gtk/gtk.h>
#endif
typedef char* AutoString;
#endif

struct Monitor
{
	struct MonitorRect
	{
		int x, y;
		int width, height;
	} monitor, work;
};

typedef void (*ACTION)();
typedef void (*WebMessageReceivedCallback)(AutoString message);
typedef void* (*WebResourceRequestedCallback)(AutoString url, int* outNumBytes, AutoString* outContentType);
typedef int (*GetAllMonitorsCallback)(const Monitor* monitor);
typedef void (*ResizedCallback)(int width, int height);
typedef void (*MovedCallback)(int x, int y);
typedef void (*ClosingCallback)();
// Suggestion:
// Add parameter that indicates whether the window is actually closing
// typedef void (*ClosingCallback)(bool close);

class Photino
{
private:
	WebMessageReceivedCallback _webMessageReceivedCallback;
	MovedCallback _movedCallback;
	ResizedCallback _resizedCallback;
	ClosingCallback _closingCallback;
#ifdef _WIN32
	static HINSTANCE _hInstance;
	HWND _hWnd;
	Photino* _parent;
	wil::com_ptr<ICoreWebView2Environment> _webviewEnvironment;
	wil::com_ptr<ICoreWebView2> _webviewWindow;
	wil::com_ptr<ICoreWebView2Controller> _webviewController;
	std::map<std::wstring, WebResourceRequestedCallback> _schemeToRequestHandler;
	void AttachWebView();
#elif OS_LINUX
	GtkWidget* _window;
	GtkWidget* _webview;
#elif OS_MAC
	void* _window;
	void* _webview;
	void* _webviewConfiguration;
	void AttachWebView();
#endif

public:
#ifdef _WIN32
	static void Register(HINSTANCE hInstance);
	HWND getHwnd();
	void RefitContent();
#elif OS_MAC
	static void Register();
#endif

	Photino(AutoString title, Photino* parent, WebMessageReceivedCallback webMessageReceivedCallback, bool fullscreen, int x, int y, int width, int height);
	~Photino();
	void SetTitle(AutoString title);
	void Show();
	void Close();
	void WaitForExit();
	void ShowMessage(AutoString title, AutoString body, unsigned int type);
	void Invoke(ACTION callback);
	void NavigateToUrl(AutoString url);
	void NavigateToString(AutoString content);
	void SendWebMessage(AutoString message);
	void AddCustomScheme(AutoString scheme, WebResourceRequestedCallback requestHandler);
	void SetResizable(bool resizable);
	void GetSize(int* width, int* height);
	void SetSize(int width, int height);
	void SetResizedCallback(ResizedCallback callback) { _resizedCallback = callback; }
	void InvokeResized(int width, int height) { if (_resizedCallback) _resizedCallback(width, height); }
	void GetAllMonitors(GetAllMonitorsCallback callback);
	unsigned int GetScreenDpi();
	void GetPosition(int* x, int* y);
	void SetPosition(int x, int y);
	void SetMovedCallback(MovedCallback callback) { _movedCallback = callback; }
	void SetClosingCallback(ClosingCallback callback) { _closingCallback = callback; }
	void InvokeMoved(int x, int y) { if (_movedCallback) _movedCallback(x, y); }
	void InvokeClosing() { if (_closingCallback) _closingCallback(); }
	// Suggestion:
    // Add parameter that indicates whether the window is actually closing
    // void InvokeClosing(bool close) { if (_closingCallback) _closingCallback(close); }
	void SetTopmost(bool topmost);
	void SetIconFile(AutoString filename);
};

#endif // !Photino_H