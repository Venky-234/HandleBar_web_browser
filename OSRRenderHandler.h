#pragma once
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_helpers.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <mutex>

class OSRRenderHandler : public CefRenderHandler {
public:
    SDL_Texture* tex;

    OSRRenderHandler(int width, int height, SDL_Renderer* ren);
    ~OSRRenderHandler();

    // Required: Return view rectangle
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override; 

    // Required: Handle paint events
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width,
        int height) override;     

    // Optional: Save frame to file
    void SaveFrameToFile(const std::string& filename); 

    std::mutex texture_mutex;
    std::unordered_map<int, SDL_Texture*> tab_textures;

    SDL_Renderer* renderer;

    /*
    // Save frame as PPM image (viewable format)
    void SaveFrameToPPM(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            // PPM header
            file << "P6\n" << width_ << " " << height_ << "\n255\n";

            // Convert BGRA to RGB
            for (int i = 0; i < width_ * height_; i++) {
                file.put(buffer_[i * 4 + 2]); // R
                file.put(buffer_[i * 4 + 1]); // G
                file.put(buffer_[i * 4 + 0]); // B
            }
            file.close();
            printf("PPM image saved to %s\n", filename.c_str());
        }
    }
    */

    // Get buffer for external rendering
    unsigned char* GetBuffer();
    int GetWidth();
    int GetHeight();

private:
    int width_;
    int height_;
    unsigned char* buffer_;
    size_t buffer_size_;
    int frame_count_ = 0;

    IMPLEMENT_REFCOUNTING(OSRRenderHandler);
};