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

            // vulkan 1.3 features
            VkPhysicalDeviceVulkan13Features features13{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
            };
            features13.dynamicRendering = true;
            features13.synchronization2 = true;

            // vulkan 1.2 features
            VkPhysicalDeviceVulkan12Features features12{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
            };
            features12.bufferDeviceAddress = true;
            features12.descriptorIndexing = true;

            // use vkbootstrap to select a gpu.
            // We want a gpu that can write to the SDL surface and supports
            // vulkan 1.3 with the correct features
            vkb::PhysicalDeviceSelector selector{ vkb_inst };
            vkb::PhysicalDevice physicalDevice =
                selector.set_minimum_version(1, 3)
                    .set_required_features_13(features13)
                    .set_required_features_12(features12)
                    .set_surface(_surface)
                    .select()
                    .value();

            vkb::DeviceBuilder deviceBuilder{ physicalDevice };
            vkb::Device vkbDevice = deviceBuilder.build().value();

            // Get the VkDevice handle used in the rest of a vulkan application
            _device = vkbDevice.device;
            _chosenGPU = physicalDevice.physical_device;

            assert(_device != VK_NULL_HANDLE);

            SPDLOG_INFO("VulkanRenderer created");
        }

        ~VulkanRenderer() { SPDLOG_INFO("VulkanRenderer destroyed"); }

        void do_frame() override {}

    private:
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;
        VkDevice _device = VK_NULL_HANDLE;
        VkPhysicalDevice _chosenGPU = VK_NULL_HANDLE;
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IRenderer> create_vulkan_renderer(
        const VulkanRendererCreateInfo& cinfo
    ) {
        return std::make_unique<VulkanRenderer>(cinfo);
    }

}  // namespace mirinae::vulkan
