#pragma once


namespace mirinae::vulkan {

    struct IRenderer {
        virtual ~IRenderer() = default;

        virtual void do_frame() = 0;
    };

}  // namespace mirinae::vulkan
