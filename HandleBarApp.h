#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/internal/cef_win.h>
#include <include/internal/cef_types_wrappers.h>
#include <include/internal/cef_ptr.h>
#include <include/internal/cef_string.h>
#include <include/cef_base.h>
#include <SDL3/SDL.h>
#include <thread>
#include <mutex>

#include <windows.h>
#include <iostream>

#include "OSRRenderHandler.h"
#include "OSRClient.h"


class HandleBarApp : public CefApp, public CefBrowserProcessHandler{
private:
    CefMainArgs main_args;
    CefBrowserSettings browser_settings; // default browser settings
    CefRefPtr<OSRRenderHandler> render_handler;
    CefRefPtr<OSRClient> client;
    CefWindowInfo window_info;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event event;
    SDL_Texture* tex;
    char buf[100] = "\0";
    //std::vector<char*>  
    float mouse_x = -1;
    float mouse_y = -1;

    CefMouseEvent mouse_event;
    CefMouseEvent prev_mouse_event;

    CefKeyEvent key_event;


    int width = 1000;
    int height = 800;

public:
    HandleBarApp();

    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnContextInitialized() override;
    void InitializeCef(std::string url); 
    bool CreateBrowser(std::string& url);
    void handle_mouse_events();
    void handle_keyboard_events();
    void render_all_ui();
    void Loop(); 

private:
    IMPLEMENT_REFCOUNTING(HandleBarApp);

};