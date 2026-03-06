#pragma once

#include <functional>
#include <memory>

#include "mirinae/vulkan/renderer.hpp"


namespace mirinae::vulkan {

    struct VulkanRendererCreateInfo {
        using SurfaceCreator = std::function<uint64_t(uint64_t)>;

        SurfaceCreator surface_creator_;
    };


    std::unique_ptr<IRenderer> create_vulkan_renderer(
        const VulkanRendererCreateInfo& cinfo
    );

}  // namespace mirinae::vulkan
