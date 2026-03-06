#include <volk.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include "mirinae/auxiliary/log.hpp"
#include "mirinae/vulkan/renderer_vulkan.hpp"


namespace {

    class WindowSDL {

    public:
        WindowSDL(int width, int height) {
            SDL_Init(SDL_INIT_VIDEO);
            auto flags = SDL_WINDOW_RESIZABLE;
            window_ = SDL_CreateWindow("Mirinae", width, height, flags);
            if (!window_) {
                SPDLOG_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
                std::abort();
            }

            SDL_ShowWindow(window_);
        }

        ~WindowSDL() {
            if (window_) {
                SDL_DestroyWindow(window_);
                window_ = nullptr;
            }
        }

        std::pair<int, int> get_win_size() const {
            int width, height;
            SDL_GetWindowSize(window_, &width, &height);
            return { width, height };
        }

        std::pair<int, int> get_fbuf_size() const {
            int width, height;
            SDL_GetWindowSizeInPixels(window_, &width, &height);
            return { width, height };
        }

        VkSurfaceKHR create_vulkan_surface(VkInstance instance) const {
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            if (!SDL_Vulkan_CreateSurface(window_, instance, 0, &surface)) {
                SPDLOG_ERROR(
                    "Failed to create Vulkan surface: {}", SDL_GetError()
                );
                return VK_NULL_HANDLE;
            }
            return surface;
        }

    private:
        SDL_Window* window_ = nullptr;
    };


    class MainApplication {

    public:
        MainApplication() : window_(1280, 720) {
            system("chcp 65001");

            mirinae::vulkan::VulkanRendererCreateInfo cinfo;
            cinfo.fbuf_w_ = window_.get_fbuf_size().first;
            cinfo.fbuf_h_ = window_.get_fbuf_size().second;
            cinfo.surface_creator_ = [this](uint64_t instance) -> uint64_t {
                auto vk_instance = reinterpret_cast<VkInstance>(instance);
                auto vk_surface = this->window_.create_vulkan_surface(
                    vk_instance
                );
                return reinterpret_cast<uint64_t>(vk_surface);
            };
            renderer_ = mirinae::vulkan::create_vulkan_renderer(cinfo);
        }

        void do_frame() { renderer_->do_frame(); }

        void on_resize(int width, int height) {
            const auto [win_width, win_height] = window_.get_win_size();
            const auto [fbuf_width, fbuf_height] = window_.get_fbuf_size();
            SPDLOG_INFO(
                "Window resized: {}x{}, {}x{}",
                win_width,
                win_height,
                fbuf_width,
                fbuf_height
            );
        }

        SDL_AppResult proc_event(const SDL_Event& e) {
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                this->on_resize(e.window.data1, e.window.data2);
            } else if (e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                this->on_resize(e.window.data1, e.window.data2);
            }

            return SDL_AppResult::SDL_APP_CONTINUE;
        }

    private:
        WindowSDL window_;
        std::unique_ptr<mirinae::vulkan::IRenderer> renderer_;
    };

}  // namespace


SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    *appstate = new ::MainApplication();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto app = static_cast<::MainApplication*>(appstate);
    delete app;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto app = static_cast<::MainApplication*>(appstate);
    app->do_frame();
    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (nullptr == event)
        return SDL_AppResult::SDL_APP_CONTINUE;
    if (event->type == SDL_EVENT_QUIT)
        return SDL_AppResult::SDL_APP_SUCCESS;

    auto app = static_cast<::MainApplication*>(appstate);
    if (nullptr == app)
        return SDL_AppResult::SDL_APP_FAILURE;

    return app->proc_event(*event);
}
