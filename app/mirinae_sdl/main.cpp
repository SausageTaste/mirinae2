#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>


namespace {

    class WindowSDL {

    public:
        WindowSDL(int width, int height) {
            SDL_Init(SDL_INIT_VIDEO);
            auto flags = SDL_WINDOW_RESIZABLE;
            window_ = SDL_CreateWindow("Mirinae", width, height, flags);
            assert(window_);

            SDL_ShowWindow(window_);
        }

        ~WindowSDL() {
            if (window_) {
                SDL_DestroyWindow(window_);
                window_ = nullptr;
            }
        }

    private:
        SDL_Window* window_ = nullptr;
    };


    class MainApplication {

    public:
        MainApplication() : window_(1280, 720) { system("chcp 65001"); }

        void do_frame() {}

        void on_resize(int width, int height) {
            SPDLOG_INFO("Window resized: {}x{}", width, height);
        }

        SDL_AppResult proc_event(const SDL_Event& e) {
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                this->on_resize(e.window.data1, e.window.data2);
            }

            return SDL_AppResult::SDL_APP_CONTINUE;
        }

    private:
        WindowSDL window_;
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
