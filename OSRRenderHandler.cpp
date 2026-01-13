#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_helpers.h>
#include <fstream>
#include <iostream>

#include "OSRRenderHandler.h"

OSRRenderHandler::OSRRenderHandler(int width, int height, SDL_Renderer* ren)
    : width_(width), height_(height) {
    // Allocate buffer for BGRA pixels
    buffer_size_ = width_ * height_ * 4;
    buffer_ = new unsigned char[buffer_size_];

    for (int i = 0; i < buffer_size_; i++) {
        buffer_[i] = 0;
    }
    renderer = ren;
}

OSRRenderHandler::~OSRRenderHandler() {
    delete[] buffer_;
}

// Required: Return view rectangle
void OSRRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    rect = CefRect(0, 0, width_, height_);
}

// Required: Handle paint events
void OSRRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    const void* buffer,
    int width,
    int height) {
    
    /*
    if (type == PET_VIEW) {
        // Copy the rendered frame to our buffer
        memcpy(buffer_, buffer, width * height * 4);

        frame_count_++;
        printf("Frame %d rendered: %dx%d, Dirty regions: %zu\n",
            frame_count_, width, height, dirtyRects.size());

        // Save first frame as example
        if (frame_count_ == 1) {
            SaveFrameToPPM("first_frame.ppm");
        }

        // You can now use buffer_ for:
        // 1. Upload to OpenGL/DirectX texture
        // 2. Process with computer vision
        // 3. Stream to network
        // 4. Video encoding
    } */
    
    //std::cout << "Painting onto memoryy !!!" << tex << std::endl;
    texture_mutex.lock();
    auto it = tab_textures.find(browser->GetIdentifier());

    if (it != tab_textures.end()) {
        // found
        SDL_UpdateTexture(it->second, NULL, buffer, width*4);
    }
    else {
        // not found
        tab_textures.insert(
            {
                browser->GetIdentifier(), 
                SDL_CreateTexture(
                    renderer, 
                    SDL_PIXELFORMAT_BGRA32,
                    SDL_TEXTUREACCESS_STREAMING, 
                    width, 
                    height 
                )
            }
        );
        SDL_UpdateTexture(tab_textures[browser->GetIdentifier()], NULL, buffer, width*4);
    }
    texture_mutex.unlock();

    /*
    bool fl = SDL_UpdateTexture(tex, NULL, buffer ,width*4);
    if (fl == false) {
        //std::cout << "Failed !!!" << std::endl;
        SDL_Log("SDL_UpdateTexture failed: %s", SDL_GetError());
    }
    */

    //std::cout << "Copying to texture" << std::endl;
}

/*
// Optional: Save frame to file
void SaveFrameToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char*>(buffer_), buffer_size_);
        file.close();
        printf("Frame saved to %s\n", filename.c_str());
    }
}
*/

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
unsigned char* OSRRenderHandler::GetBuffer() { return buffer_; }
int OSRRenderHandler::GetWidth() { return width_; }
int OSRRenderHandler::GetHeight() { return height_; }
