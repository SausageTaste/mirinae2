#include "mirinae/vulkan/renderer_vulkan.hpp"

#include <volk.h>

#include "device/device.hpp"
#include "mirinae/auxiliary/log.hpp"


namespace {

    class VulkanRenderer : public mirinae::vulkan::IRenderer {

    public:
        VulkanRenderer(const mirinae::vulkan::VulkanRendererCreateInfo& cinfo) {
            _device = mirinae::vulkan::create_vulkan_device(cinfo);
        }

        ~VulkanRenderer() { SPDLOG_INFO("VulkanRenderer destroyed"); }

        void do_frame() override {}

    private:
        std::unique_ptr<mirinae::vulkan::IDevice> _device;
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer(
        const VulkanRendererCreateInfo& cinfo
    ) {
        return std::make_unique<VulkanRenderer>(cinfo);
    }

}  // namespace mirinae::vulkan
