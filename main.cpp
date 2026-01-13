//#define NOMINMAX
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/internal/cef_win.h>
#include <include/internal/cef_types_wrappers.h>
#include <include/internal/cef_ptr.h>
#include <include/internal/cef_string.h>
#include <include/cef_base.h>
//#incldue <include/cef_version.h>

#include <windows.h>
//#include <iostream>
#include <iostream>

#include "HandleBarApp.h"
#include "OSRRenderHandler.h"
#include "OSRClient.h"

#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
    std::cout << "Entry point ..." << std::endl;
    CefMainArgs main_args(GetModuleHandle(NULL));
    CefRefPtr<HandleBarApp> app(new HandleBarApp());

    // You MUST specify the path to the resources folder!
	// CefString(&settings.resources_dir_path) = L"D:\\handle_bar\\cef_binaries"; // This part does not work properly 
    //CefString(&settings.locales_dir_path) = L"D:\\handle_bar\\cef_binaries\\Resources\\locales";
    // CEF's multi-process architecture requires this check.

    CefSettings settings;
    settings.no_sandbox = true; // for OSR
    settings.windowless_rendering_enabled = true; // for OSR
    //settings.file_access_from_file_urls = STATE_ENABLED;
    //settings.universal_access_from_file_urls = STATE_ENABLED;


    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
        return exit_code; // Child process executed, exit now.
    }

    // Initialize CEF
    if (CefInitialize(main_args, settings, app.get(), nullptr)) {
        app->InitializeCef("file:///D:\\handle_bar_vsc\\starting_page.html");
    }
    std::cout << "Cef execution is over ..." << std::endl;
    // Cleanup
    CefShutdown();
    return 0;
}