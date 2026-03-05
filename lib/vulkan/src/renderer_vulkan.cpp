#include "mirinae/vulkan/renderer_vulkan.hpp"

#include <volk.h>

#include <VkBootstrap.h>

#include "mirinae/auxiliary/log.hpp"


namespace {

    class VulkanRenderer : public mirinae::vulkan::IRenderer {

    public:
        VulkanRenderer(const mirinae::vulkan::VulkanRendererCreateInfo& cinfo) {
            if (VK_SUCCESS != volkInitialize()) {
                SPDLOG_ERROR("Failed to initialize volk");
            }

            vkb::InstanceBuilder builder;

            // make the vulkan instance, with basic debug features
            auto inst_ret = builder.set_app_name("Example Vulkan Application")
                                .request_validation_layers(true)
                                .use_default_debug_messenger()
                                .require_api_version(1, 3, 0)
                                .build();

            vkb::Instance vkb_inst = inst_ret.value();

            // grab the instance
            _instance = vkb_inst.instance;
            _debug_messenger = vkb_inst.debug_messenger;
            _surface = static_cast<VkSurfaceKHR>(
                cinfo.surface_creator_(_instance)
            );

            assert(_instance != VK_NULL_HANDLE);
            assert(_surface != VK_NULL_HANDLE);

            SPDLOG_INFO("VulkanRenderer created");
        }

        ~VulkanRenderer() { SPDLOG_INFO("VulkanRenderer destroyed"); }

        void do_frame() override {}

    private:
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer(
        const VulkanRendererCreateInfo& cinfo
    ) {
        return std::make_unique<VulkanRenderer>(cinfo);
    }

}  // namespace mirinae::vulkan
