#include "device/device.hpp"

#include <VkBootstrap.h>

#include "mirinae/auxiliary/log.hpp"


// Device
namespace mirinae::vulkan {

    Device::Device() {}

    Device::~Device() { this->destroy(); }

    bool Device::init(const VulkanRendererCreateInfo& cinfo) {
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

    void Device::destroy() {
        if (surface_ != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(instance_, surface_, nullptr);

        if (device_ != VK_NULL_HANDLE)
            vkDestroyDevice(device_, nullptr);

        if (debug_messenger_ != VK_NULL_HANDLE)
            vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);

        if (instance_ != VK_NULL_HANDLE)
            vkDestroyInstance(instance_, nullptr);
    }

}


// Swapchain
namespace mirinae::vulkan {

    Swapchain::Swapchain() {}

    Swapchain::~Swapchain() {}

    bool Swapchain::init(uint32_t width, uint32_t height, Device& device) {
        vkb::SwapchainBuilder swapchainBuilder{ device.phys_device(),
                                                device.logi_device(),
                                                device.surface() };

        img_format_ = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain =
            swapchainBuilder
                //.use_default_format_selection()
                .set_desired_format(
                    VkSurfaceFormatKHR{
                        .format = img_format_,
                        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
                )
                // use vsync present mode
                .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                .set_desired_extent(width, height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        extent_ = vkbSwapchain.extent;
        // store swapchain and its related images
        swapchain_ = vkbSwapchain.swapchain;
        images_ = vkbSwapchain.get_images().value();
        img_views_ = vkbSwapchain.get_image_views().value();

        return true;
    }

    void Swapchain::destroy(Device& device) {
        for (auto view : img_views_)
            vkDestroyImageView(device.logi_device(), view, nullptr);
        img_views_.clear();

        if (swapchain_ != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(device.logi_device(), swapchain_, nullptr);
    }

}  // namespace mirinae::vulkan
