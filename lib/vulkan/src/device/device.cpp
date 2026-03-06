#include "device/device.hpp"

#include <volk.h>

#include <VkBootstrap.h>

#include "mirinae/auxiliary/log.hpp"


namespace {

    class Device : public mirinae::vulkan::IDevice {

    public:
        bool init(const mirinae::vulkan::VulkanRendererCreateInfo& cinfo) {
            if (VK_SUCCESS != volkInitialize()) {
                SPDLOG_ERROR("Failed to initialize volk");
                return false;
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
            instance_ = vkb_inst.instance;
            debug_messenger_ = vkb_inst.debug_messenger;

            static_assert(sizeof(uint64_t) == sizeof(VkSurfaceKHR));
            static_assert(sizeof(uint64_t) == sizeof(VkInstance));
            surface_ = reinterpret_cast<VkSurfaceKHR>(
                cinfo.surface_creator_(reinterpret_cast<uint64_t>(instance_))
            );

            assert(instance_ != VK_NULL_HANDLE);
            assert(surface_ != VK_NULL_HANDLE);

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
                    .set_surface(surface_)
                    .select()
                    .value();

            vkb::DeviceBuilder deviceBuilder{ physicalDevice };
            vkb::Device vkbDevice = deviceBuilder.build().value();

            // Get the VkDevice handle used in the rest of a vulkan application
            device_ = vkbDevice.device;
            chosenGPU_ = physicalDevice.physical_device;

            assert(device_ != VK_NULL_HANDLE);

            return true;
        }

    private:
        VkInstance instance_ = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;
        VkPhysicalDevice chosenGPU_ = VK_NULL_HANDLE;
    };

}  // namespace


namespace mirinae::vulkan {

    std::unique_ptr<IDevice> create_vulkan_device(
        const VulkanRendererCreateInfo& cinfo
    ) {
        auto device = std::make_unique<Device>();
        if (device->init(cinfo))
            return device;

        return nullptr;
    }

}  // namespace mirinae::vulkan
