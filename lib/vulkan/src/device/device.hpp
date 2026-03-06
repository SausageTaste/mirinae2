#pragma once

#include <memory>

#include <volk.h>

#include "mirinae/vulkan/cinfo.hpp"


namespace mirinae::vulkan {

    class Device {

    public:
        Device();
        ~Device();

        bool init(const VulkanRendererCreateInfo& cinfo);
        void destroy();

        VkPhysicalDevice phys_device() const { return chosenGPU_; }
        VkDevice logi_device() const { return device_; }
        VkSurfaceKHR surface() const { return surface_; }

    private:
        VkInstance instance_ = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;
        VkPhysicalDevice chosenGPU_ = VK_NULL_HANDLE;
    };


    class Swapchain {

    public:
        Swapchain();
        ~Swapchain();

        bool init(uint32_t width, uint32_t height, Device& device);
        void destroy(Device& device);

    private:
        VkSwapchainKHR swapchain_;
        VkFormat img_format_;

        std::vector<VkImage> images_;
        std::vector<VkImageView> img_views_;
        VkExtent2D extent_;
    };

}  // namespace mirinae::vulkan
