#pragma once

#include <functional>


namespace mirinae::vulkan {

    struct VulkanRendererCreateInfo {
        using SurfaceCreator = std::function<uint64_t(uint64_t)>;

        SurfaceCreator surface_creator_ = nullptr;
        int fbuf_w_ = 0;
        int fbuf_h_ = 0;
    };

}  // namespace mirinae::vulkan
