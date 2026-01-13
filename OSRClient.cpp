#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_helpers.h>
#include <fstream>

#include "OSRClient.h"

#include "OSRRenderHandler.h"

OSRClient::OSRClient(CefRefPtr<OSRRenderHandler> render_handler) 
    : render_handler_(render_handler){}

CefRefPtr<CefRenderHandler> OSRClient::GetRenderHandler() {
    return render_handler_;
}

CefRefPtr<CefLifeSpanHandler> OSRClient::GetLifeSpanHandler(){
    return this;
}

CefRefPtr<CefLoadHandler> OSRClient::GetLoadHandler(){
    return this;
}

void OSRClient::OnAfterCreated(CefRefPtr<CefBrowser> browser){
    CEF_REQUIRE_UI_THREAD();

    tab_mutex.lock();
    browser_list_.push_back(browser);
    tab_mutex.unlock();

    std::cout << "browser_list_size" << browser_list_.size() << std::endl; 

    browser->GetHost()->SetFocus(true);
    browser->GetHost()->WasResized();
    browser->GetHost()->SetAccessibilityState(STATE_ENABLED);

    printf("Browser created (OSR mode)\n");
}

void OSRClient::OnBeforeClose(CefRefPtr<CefBrowser> browser){
    CEF_REQUIRE_UI_THREAD();

    for (auto it = browser_list_.begin(); it != browser_list_.end(); ++it) {
        if ((*it)->IsSame(browser)) {
            browser_list_.erase(it);
            break;
        }
    }

    if (browser_list_.empty()) {
        CefQuitMessageLoop();
    }
}

// CefLoadHandler methods
void OSRClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    TransitionType transition_type) {
    CEF_REQUIRE_UI_THREAD();
    if (frame->IsMain()) {
        printf("Loading started: %s\n", frame->GetURL().ToString().c_str());
    }
}

void OSRClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int httpStatusCode) {
    CEF_REQUIRE_UI_THREAD();
    if (frame->IsMain()) {
        printf("Loading finished: %s (Status: %d)\n",
            frame->GetURL().ToString().c_str(), httpStatusCode);
    }
}

void OSRClient::OnLoadError(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl) {
    CEF_REQUIRE_UI_THREAD();
    if (errorCode == ERR_ABORTED) return;

    printf("Load error: %s (Error: %s)\n",
        failedUrl.ToString().c_str(),
        errorText.ToString().c_str());
}

CefRefPtr<CefBrowser> OSRClient::GetBrowser() {
    if (browser_list_.empty()) {
        return nullptr;
    }
    else {
        return browser_list_[current_tab];
    }
    //return browser_list_.empty() ? nullptr : browser_list_.front();
}

int OSRClient::GetBrowserListSize() {
    return browser_list_.size();
}