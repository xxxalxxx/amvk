#ifndef AMVK_VULKAN_IMAGE_MANAGER_H
#define AMVK_VULKAN_IMAGE_MANAGER_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include "cmd_pass.h"
#include "vulkan_state.h"
#include "vulkan_utils.h"
#include "vulkan_buffer_creator.h"
#include "macro.h"

class VulkanImageDesc {
public:
	VulkanImageDesc(const VkDevice& vkDevice);
	~VulkanImageDesc();
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory memory;
private:
	const VkDevice& mVkDevice;
};

struct VulkanTexture {
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkDeviceMemory deviceMemory;
	VkImageView view;
	uint32_t width, height;
	uint32_t mipLevels;
	uint32_t layerCount;
	VkDescriptorImageInfo descriptor;
};

class VulkanImageCreator {
public:
	VulkanImageCreator(const VulkanState& vulkanState);
	void createImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
	void copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height) const;
	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView) const;
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	VkFormat findDepthFormat() const;
private:
	const VulkanState& mVulkanState;
};

#endif