#pragma once

#include <memory>

#include "mirinae/vulkan/cinfo.hpp"


namespace mirinae::vulkan {

    struct IDevice {
        virtual ~IDevice() = default;
    };

    std::unique_ptr<IDevice> create_vulkan_device(
        const VulkanRendererCreateInfo& cinfo
    );

}  // namespace mirinae::vulkan
