#include <iostream>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/internal/cef_win.h>
#include <include/internal/cef_types_wrappers.h>
#include <include/internal/cef_ptr.h>
#include <include/internal/cef_string.h>
#include <include/cef_base.h>

#include <windows.h>
#include <iostream>

#include "HandleBarApp.h"
#include "OSRRenderHandler.h"
#include "OSRClient.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

CefMainArgs main_args;
CefRefPtr<HandleBarApp> app;
CefSettings settings;

bool CefInitialized = false;

void InitializeHandleBar() {
    int WIDTH = 800;
    int HEIGHT = 600;

    main_args = CefMainArgs(GetModuleHandle(NULL));
    app = CefRefPtr<HandleBarApp>(new HandleBarApp(WIDTH, HEIGHT));
    settings.no_sandbox = true; // for OSR
    settings.windowless_rendering_enabled = true; // for OSR

    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
        std::cout << "Process exited with code : " << exit_code << std::endl; 
        return; // Child process executed, exit now.
    }

    // Initialize CEF
    if (CefInitialize(main_args, settings, app.get(), nullptr)) {
        CefInitialized = false;
        app->InitializeCef("www.google.com");
    }
    
}
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    //SDL_SetAppMetadata("Example Renderer Clear", "1.0", "hello world");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("handle bar window", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    InitializeHandleBar();


    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{

    const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
    /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
    const float red = (float) (0.5 + 0.5 * SDL_sin(now));
    const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);
    
    std::cout << "hello world" << std::endl;
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    std::cout << "Cef execution is over ..." << std::endl;

    // Cleanup
    CefShutdown();
    /* SDL will clean up the window/renderer for us. */
}