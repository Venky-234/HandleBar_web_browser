#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/internal/cef_win.h>
#include <include/internal/cef_types_wrappers.h>
#include <include/internal/cef_ptr.h>
#include <include/internal/cef_string.h>
#include <include/cef_base.h>

#include <windows.h>
#include <iostream>

#include "OSRRenderHandler.h"
#include "OSRClient.h"
#include "HandleBarApp.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

HandleBarApp::HandleBarApp() {}

CefRefPtr<CefBrowserProcessHandler> HandleBarApp::GetBrowserProcessHandler(){ 
    return this; 
}

void initialize_ImGui(SDL_Window* window, SDL_Renderer* renderer);
void HandleBarApp::OnContextInitialized() {
    std::cout << "OnContextInitialized is being run ..." << std::endl;

    browser_settings.windowless_frame_rate = 30;  // Max 30 FPS
    window_info.SetAsWindowless(0);  // Set windowless (OSR) mode // Pass parent window handle if needed


    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        //return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("handle bar window", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        //return SDL_APP_FAILURE;
    }

    else {
        render_handler = new OSRRenderHandler(width, height, renderer);
        client = new OSRClient(render_handler);
        initialize_ImGui(window, renderer);
        tex = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_BGRA32,
            SDL_TEXTUREACCESS_STREAMING, 
            width, 
            height 
        );

        if (tex == NULL) {
            std::cout << "Failed to create texture" << std::endl;
        }
        else {
            std::cout << tex << std::endl;
        }

        render_handler->tex = tex;
    }
}
void SetupImGuiStyle()
{
    // Get the current style pointer
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- Rounding & Geometry ---
    // Modern UI tends to have softer, rounded corners.
    style.WindowRounding    = 8.0f;   // Corners of main windows
    style.ChildRounding     = 6.0f;   // Corners of child windows/panels
    style.FrameRounding     = 5.0f;   // Corners of input fields, checkboxes, buttons
    style.PopupRounding     = 6.0f;   // Corners of combos, menus
    style.ScrollbarRounding = 9.0f;   // Rounded scrollbars
    style.GrabRounding      = 5.0f;   // Corners of sliders and resize grips
    style.TabRounding       = 6.0f;   // Corners of tabs

    style.WindowBorderSize  = 1.0f;   // Thin subtle border around windows
    style.ChildBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;   // Slight border on inputs for definition
    style.PopupBorderSize   = 1.0f;

    // Increase padding for a less cramped, more "breathable" look
    style.WindowPadding     = ImVec2(10.0f, 10.0f);
    style.FramePadding      = ImVec2(6.0f,  4.0f);
    style.ItemSpacing       = ImVec2(8.0f,  6.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f,  4.0f);
    style.ScrollbarSize     = 14.0f;  // Slightly thinner scrollbars

    // --- Colors (The "Dark Slate Blue" Palette) ---

    // Core Base Colors
    // Use off-white text for better readability against dark backgrounds
    colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);
    // Disabled text color
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.55f, 0.57f, 0.60f, 1.00f);

    // Backgrounds (Deep charcoal/slate blues instead of flat gray)
    colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.12f, 0.15f, 0.98f);

    // Borders (Subtle, to define edges without shouting)
    colors[ImGuiCol_Border]                 = ImVec4(0.22f, 0.24f, 0.28f, 0.80f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frame Backgrounds (Inputs, Checkboxes)
    // Lighter than window BG to stand out
    colors[ImGuiCol_FrameBg]                = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.22f, 0.24f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.28f, 0.33f, 1.00f);

    // Title Bar
    colors[ImGuiCol_TitleBg]                = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
    // When the window is active, give the titlebar a subtle blue tint
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.14f, 0.18f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.11f, 0.12f, 0.15f, 0.75f);

    // --- The Accent Color ---
    // A nice professional blue: (R:52, G:120, B:215) -> (0.20f, 0.47f, 0.84f)
    // Used for active elements, highlights, and checks.
    ImVec4 accentColor = ImVec4(0.20f, 0.47f, 0.84f, 1.00f);
    ImVec4 accentHover = ImVec4(0.26f, 0.54f, 0.92f, 1.00f); // Slightly lighter
    ImVec4 accentActive = ImVec4(0.17f, 0.40f, 0.73f, 1.00f); // Slightly darker

    colors[ImGuiCol_CheckMark]              = accentColor;
    colors[ImGuiCol_SliderGrab]             = accentColor;
    colors[ImGuiCol_SliderGrabActive]       = accentActive;

    // Buttons (Neutral default, accented on hover/active)
    colors[ImGuiCol_Button]                 = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = accentHover;
    colors[ImGuiCol_ButtonActive]           = accentActive;

    // Headers (Used in menus, collapsing headers, tree nodes)
    colors[ImGuiCol_Header]                 = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = accentHover;
    colors[ImGuiCol_HeaderActive]           = accentActive;

    // Tabs
    colors[ImGuiCol_Tab]                    = ImVec4(0.14f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered]             = accentHover;
    colors[ImGuiCol_TabActive]              = accentColor;
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.14f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);

    // Use the accent color for standard text selection highlight
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.35f);
    
    // Other elements
    colors[ImGuiCol_Separator]              = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = accentHover;
    colors[ImGuiCol_SeparatorActive]        = accentActive;
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.22f, 0.24f, 0.28f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]      = accentHover;
    colors[ImGuiCol_ResizeGripActive]       = accentActive;
    colors[ImGuiCol_PlotLines]              = accentColor;
    colors[ImGuiCol_PlotLinesHovered]       = accentHover;
    colors[ImGuiCol_PlotHistogram]          = accentColor;
    colors[ImGuiCol_PlotHistogramHovered]   = accentHover;

    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.26f, 0.28f, 0.33f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.28f, 0.30f, 0.35f, 1.00f);
}

void initialize_ImGui(SDL_Window* window, SDL_Renderer* renderer) {
    std::cout << "ImGui UI is initializing ..." << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    SetupImGuiStyle();
    
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void HandleBarApp::InitializeCef(std::string url) {
    std::cout << "HandleBar is starting ..." << std::endl;

    // Create new browser instance
    CreateBrowser(url); 
    //CreateBrowser(std::string("www.google.com")); 
    //CreateBrowser(std::string("www.chatgpt.com")); 
    //CreateBrowser(url); 

    Loop();
}

bool HandleBarApp::CreateBrowser(std::string& url) {
    return CefBrowserHost::CreateBrowser(
        window_info,
        client.get(),
        url,
        browser_settings,
        nullptr,
        nullptr
    );

}

void HandleBarApp::render_all_ui() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    
    //ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_Always);
    //ImGui::SetWindowPos(ImVec2(500, 0), ImGuiCond_Always);

    ImGui::Begin("New tab or something ?");

    //ImGui::Text("go to website bro");               // Display some text (you can use a format strings too)
    ImGui::InputText("enter link :", buf, IM_COUNTOF(buf));

    if(ImGui::Button("go")) {
        printf("buffer content : %s \n", buf);
        client->GetBrowser()->GetMainFrame()->LoadURL(buf);        
        client->GetBrowser()->GetHost()->SetFocus(buf);        
    }

    if (ImGui::Button("next")) {
        client->GetBrowser()->GetHost()->WasHidden(true);
        client->current_tab += 1;
        client->GetBrowser()->GetHost()->WasHidden(false);
    }

    if (ImGui::Button("prev")) {
        client->GetBrowser()->GetHost()->WasHidden(true);
        client->current_tab -= 1;
        client->GetBrowser()->GetHost()->WasHidden(false);
    }

    if (ImGui::Button("Create")) {
        client->tab_mutex.lock();
        int prev = client->GetBrowserListSize();
        client->tab_mutex.unlock();

        bool valid = CreateBrowser(std::string("file:///D:\\handle_bar_vsc\\starting_page.html")); 
        std::cout << "Create Browser called ..." << valid << std::endl;
        
        int curr = prev;
        
        while (prev == curr) {
            CefDoMessageLoopWork();
            client->tab_mutex.lock();
            curr = client->GetBrowserListSize();
            client->tab_mutex.unlock();

            if (prev != curr) {
                client->GetBrowser()->GetHost()->WasHidden(true);
                client->current_tab = curr - 1;
                client->GetBrowser()->GetHost()->WasHidden(false);
            }

        }
    }


    //ImGui::ShowStyleEditor();
    ImGui::End();
    ImGui::Render();
} 

int conv_SDLcode_to_VKcode(int key) {
    switch (key) {
        // Arrow keys
        case SDLK_LEFT:   return 0x25;  // VK_LEFT
        case SDLK_UP:     return 0x26;  // VK_UP
        case SDLK_RIGHT:  return 0x27;  // VK_RIGHT
        case SDLK_DOWN:   return 0x28;  // VK_DOWN
        
        // Navigation
        case SDLK_HOME:   return 0x24;  // VK_HOME
        case SDLK_END:    return 0x23;  // VK_END
        case SDLK_PAGEUP: return 0x21;  // VK_PRIOR
        case SDLK_PAGEDOWN: return 0x22;  // VK_NEXT
        
        // Editing
        case SDLK_BACKSPACE: return 0x08;  // VK_BACK
        case SDLK_TAB:       return 0x09;  // VK_TAB
        case SDLK_RETURN:    return 0x0D;  // VK_RETURN
        case SDLK_DELETE:    return 0x2E;  // VK_DELETE
        case SDLK_ESCAPE:    return 0x1B;  // VK_ESCAPE
        case SDLK_SPACE:     return 0x20;  // VK_SPACE
        
        // Letters (A-Z)
        case SDLK_A: return 0x41;
        case SDLK_B: return 0x42;
        case SDLK_C: return 0x43;
        case SDLK_D: return 0x44;
        case SDLK_E: return 0x45;
        case SDLK_F: return 0x46;
        case SDLK_G: return 0x47;
        case SDLK_H: return 0x48;
        case SDLK_I: return 0x49;
        case SDLK_J: return 0x4A;
        case SDLK_K: return 0x4B;
        case SDLK_L: return 0x4C;
        case SDLK_M: return 0x4D;
        case SDLK_N: return 0x4E;
        case SDLK_O: return 0x4F;
        case SDLK_P: return 0x50;
        case SDLK_Q: return 0x51;
        case SDLK_R: return 0x52;
        case SDLK_S: return 0x53;
        case SDLK_T: return 0x54;
        case SDLK_U: return 0x55;
        case SDLK_V: return 0x56;
        case SDLK_W: return 0x57;
        case SDLK_X: return 0x58;
        case SDLK_Y: return 0x59;
        case SDLK_Z: return 0x5A;
        
        // Numbers (0-9)
        case SDLK_0: return 0x30;
        case SDLK_1: return 0x31;
        case SDLK_2: return 0x32;
        case SDLK_3: return 0x33;
        case SDLK_4: return 0x34;
        case SDLK_5: return 0x35;
        case SDLK_6: return 0x36;
        case SDLK_7: return 0x37;
        case SDLK_8: return 0x38;
        case SDLK_9: return 0x39;
        
        // Modifiers
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:   return 0x10;  // VK_SHIFT
        case SDLK_LCTRL:
        case SDLK_RCTRL:    return 0x11;  // VK_CONTROL
        case SDLK_LALT:
        case SDLK_RALT:     return 0x12;  // VK_MENU
        
        // Function keys
        case SDLK_F1:  return 0x70;  // VK_F1
        case SDLK_F2:  return 0x71;
        case SDLK_F3:  return 0x72;
        case SDLK_F4:  return 0x73;
        case SDLK_F5:  return 0x74;
        case SDLK_F6:  return 0x75;
        case SDLK_F7:  return 0x76;
        case SDLK_F8:  return 0x77;
        case SDLK_F9:  return 0x78;
        case SDLK_F10: return 0x79;
        case SDLK_F11: return 0x7A;
        case SDLK_F12: return 0x7B;
        
        default: return 0;
    }
}

uint32_t GetCefModifiersFromSDL(SDL_Keymod mod) {
    uint32_t modifiers = 0;
    
    if (mod & SDL_KMOD_SHIFT) {
        modifiers |= EVENTFLAG_SHIFT_DOWN;
        std::cout << "here here shift is down" << std::endl;
    }
    if (mod & SDL_KMOD_CTRL)
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (mod & SDL_KMOD_ALT)
        modifiers |= EVENTFLAG_ALT_DOWN;
    if (mod & SDL_KMOD_CAPS)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;
    if (mod & SDL_KMOD_NUM)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    
    return modifiers;
}

void HandleBarApp::Loop() {
    key_event.native_key_code = 0;

    while (true) {
        CefDoMessageLoopWork();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                //running = false;
                CefShutdown();
                exit(0);
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
                    client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, MBT_LEFT, false, 1);
                    client->GetBrowser()->GetHost()->SetFocus(true);
                    std::cout << "Setting focus ..." << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouse_event.modifiers = EVENTFLAG_RIGHT_MOUSE_BUTTON;
                    client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, MBT_RIGHT, false, 1);
                }
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouse_event.modifiers = 0;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, MBT_LEFT, true, 1);
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, MBT_RIGHT, true, 1);
                }
            }
            
            if (event.type == SDL_EVENT_KEY_DOWN) {
                //std::cout << "here" << std::endl;
                //key_event.windows_key_code = event.key.key;
                //key_event.native_key_code = event.key.scancode;
                //key_event.character = (char)event.key.key;
                
                /*
                std::cout << (char)event.key.key << std::endl;
                std::cout << event.key.scancode << std::endl;
                std::cout << std::endl;
                */

                key_event.modifiers = GetCefModifiersFromSDL(event.key.mod);
                if (event.key.key >= 32 && event.key.key < 127 && !(event.key.mod & (EVENTFLAG_CONTROL_DOWN | EVENTFLAG_ALT_DOWN | EVENTFLAG_COMMAND_DOWN))) { 
                    key_event.type = KEYEVENT_CHAR;

                    key_event.windows_key_code = event.key.key;

                    if (event.key.mod & SDL_KMOD_SHIFT)
                        key_event.windows_key_code -= 32;

                    key_event.character = (char)event.key.key;
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);

                    key_event.type = KEYEVENT_RAWKEYDOWN;
                    key_event.windows_key_code = conv_SDLcode_to_VKcode(event.key.key);
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
                }
                
                else {
                    key_event.windows_key_code = conv_SDLcode_to_VKcode(event.key.key);
                    key_event.native_key_code = event.key.scancode;
                    key_event.modifiers = GetCefModifiersFromSDL(event.key.mod);

                    key_event.type = KEYEVENT_RAWKEYDOWN;
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
                }
                
                /*
                const bool* state = SDL_GetKeyboardState(NULL);
                if (
                    state[SDL_SCANCODE_LEFT] ||
                    state[SDL_SCANCODE_RIGHT] ||
                    state[SDL_SCANCODE_UP] ||
                    state[SDL_SCANCODE_DOWN]
                ) {
                    
                    key_event.type = KEYEVENT_RAWKEYDOWN;
                    key_event.native_key_code = event.key.scancode;
                    //key_event.character = (char)event.key.key;

                    if (state[SDL_SCANCODE_LEFT]) {
                        key_event.windows_key_code = 0x25;
                    }
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);

                }

                else {
                    key_event.windows_key_code = event.key.key;
                    key_event.native_key_code = event.key.scancode;
                    key_event.character = (char)event.key.key;

                    key_event.type = KEYEVENT_RAWKEYDOWN;
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);

                    key_event.type = KEYEVENT_CHAR;
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
                }
                */
            }
            
            if (event.type == SDL_EVENT_KEY_UP) {
                key_event.type = KEYEVENT_KEYUP;
                client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
            }

            //std::cout << "runs second" << std::endl;
            // ImGui bullshit
            ImGui_ImplSDL3_ProcessEvent(&event);

        }
        
        SDL_MouseButtonFlags button_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
        //SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_event.x = (int)mouse_x;
        mouse_event.y = (int)mouse_y;

        if (button_flags & SDL_BUTTON_LMASK){
            //std::cout << "left" << std::endl;
            mouse_event.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        }

        if (button_flags & SDL_BUTTON_RMASK) {
            //std::cout << "right" << std::endl;
            mouse_event.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
        }
        
        if (
            !(prev_mouse_event.x == mouse_event.x &&
            prev_mouse_event.y == mouse_event.y &&
            prev_mouse_event.modifiers == mouse_event.modifiers)
            
        ) {
            //std::cout << "Mouse state changed" << std::endl;
            client->GetBrowser()->GetHost()->SendMouseMoveEvent(mouse_event, false);
        }

        prev_mouse_event = mouse_event;

        render_all_ui();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        //SDL_RenderTexture(renderer, tex, NULL, NULL); 
        client->tab_mutex.lock();
        render_handler->texture_mutex.lock();
        
        auto it = render_handler->tab_textures.find(client->GetBrowser()->GetIdentifier());
        if (it != render_handler->tab_textures.end()) {
            SDL_RenderTexture(renderer, it->second, NULL, NULL);
        }
        client->tab_mutex.unlock();
        render_handler->texture_mutex.unlock();

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer); 

        Sleep(33); // hard coded (capped) to 30 fps (have to change that)
    }
        
}

// test links
// https://www.youtube.com/watch?v=xvFZjo5PgG0
// https://amethyst-dev-os.lovable.app
// google.com