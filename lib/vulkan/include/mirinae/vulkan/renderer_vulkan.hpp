#pragma once

#include <memory>

#include "mirinae/vulkan/renderer.hpp"


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer();

}  // namespace mirinae::vulkan
