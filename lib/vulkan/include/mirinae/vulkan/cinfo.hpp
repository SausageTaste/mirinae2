#pragma once

#include <functional>


namespace mirinae::vulkan {

    struct VulkanRendererCreateInfo {
        using SurfaceCreator = std::function<uint64_t(uint64_t)>;

        SurfaceCreator surface_creator_;
    };

}  // namespace mirinae::vulkan
