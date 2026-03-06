#include "mirinae/vulkan/renderer_vulkan.hpp"

#include <volk.h>

#include "device/device.hpp"
#include "mirinae/auxiliary/log.hpp"


namespace {

    class VulkanRenderer : public mirinae::vulkan::IRenderer {

    public:
        VulkanRenderer(const mirinae::vulkan::VulkanRendererCreateInfo& cinfo) {
            if (!device_.init(cinfo)) {
                SPDLOG_ERROR("Failed to initialize Vulkan device");
                std::abort();
            }

            if (!swapchain_.init(cinfo.fbuf_w_, cinfo.fbuf_h_, device_)) {
                SPDLOG_ERROR("Failed to initialize Vulkan swapchain");
                std::abort();
            }
        }

        ~VulkanRenderer() {
            swapchain_.destroy(device_);
            device_.destroy();
        }

        void do_frame() override {}

    private:
        mirinae::vulkan::Device device_;
        mirinae::vulkan::Swapchain swapchain_;
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer(
        const VulkanRendererCreateInfo& cinfo
    ) {
        return std::make_unique<VulkanRenderer>(cinfo);
    }

}  // namespace mirinae::vulkan
