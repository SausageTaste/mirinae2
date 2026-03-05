#include "mirinae/vulkan/renderer_vulkan.hpp"

#include <volk.h>

#include "mirinae/auxiliary/log.hpp"


namespace {

    class VulkanRenderer : public mirinae::vulkan::IRenderer {

    public:
        VulkanRenderer() {
            if (VK_SUCCESS != volkInitialize()) {
                SPDLOG_ERROR("Failed to initialize volk");
            }

            SPDLOG_INFO("VulkanRenderer created");
        }

        ~VulkanRenderer() { SPDLOG_INFO("VulkanRenderer destroyed"); }

        void do_frame() override {}
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer() {
        return std::make_unique<VulkanRenderer>();
    }

}  // namespace mirinae::vulkan
