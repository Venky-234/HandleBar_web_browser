#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_helpers.h>
#include "OSRRenderHandler.h"
#include <fstream>
#include <thread>
#include <mutex>

class OSRClient : 
	public CefClient, 
	public CefLifeSpanHandler, 
	public CefLoadHandler {
		
	public:
		OSRClient(CefRefPtr<OSRRenderHandler> render_handler);

		virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override; 
		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
		virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			TransitionType transition_type) override;

		virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int httpStatusCode) override;

		virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl) override;

		int GetBrowserListSize();

		std::mutex tab_mutex;
		CefRefPtr<CefBrowser> GetBrowser();
		int current_tab = 0;


	private:
		std::vector<CefRefPtr<CefBrowser>> browser_list_;
		CefRefPtr<OSRRenderHandler> render_handler_;

		IMPLEMENT_REFCOUNTING(OSRClient);
};

