#ifndef AMVK_VULKAN_IMAGE_MANAGER_H
#define AMVK_VULKAN_IMAGE_MANAGER_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include "cmd_pass.h"
#include "vulkan_state.h"
#include "vulkan_utils.h"
#include "buffer_helper.h"
#include "macro.h"

class VulkanImageDesc {
public:
	VulkanImageDesc(const VkDevice& vkDevice);
	VulkanImageDesc(const VkDevice& vkDevice, uint32_t width, uint32_t height);
	~VulkanImageDesc();
	uint32_t width, height;
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

class ImageHelper {
public:
	ImageHelper(const VulkanState& vulkanState);
	void createImage(
			uint32_t w, 
			uint32_t h, 
			VkFormat format, 
			VkImageTiling tiling,
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkImage& image, 
			VkDeviceMemory& imageMemory) const;

	static void createImage(
			const VulkanState& state,
			VulkanImageDesc& imageDesc, 
			VkFormat format, 
			VkImageTiling tiling,
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties);

	static void copyImage(
			const VulkanState& state, 
			VkImage srcImage, 
			VkImage dstImage, 
			uint32_t width, 
			uint32_t height);
	
	static void copyImage(
			const VulkanState& state, 
			VulkanImageDesc& srcImage, 
			VulkanImageDesc& dstImage);

	void transitionImageLayout(
			VkImage image, 
			VkFormat format, 
			VkImageLayout oldLayout, 
			VkImageLayout newLayout) const;

	void copyImage(
			VkImage srcImage, 
			VkImage dstImage, 
			uint32_t width, 
			uint32_t height) const;
	
	void createImageView(
			VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags, 
			VkImageView& imageView) const;
	
	VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates, 
			VkImageTiling tiling, 
			VkFormatFeatureFlags features) const;

	VkFormat findDepthFormat() const;

	static VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);

	static VkFormat findSupportedFormat(
			const VkPhysicalDevice& physicalDevice, 
			const std::vector<VkFormat>& candidates, 
			VkImageTiling tiling, 
			VkFormatFeatureFlags features);

	static void transitionLayout(
		const VulkanState& state,
		VkImage image, 
		VkFormat format, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout,
		VkImageAspectFlags barrierAspectMask,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask);

private:
	const VulkanState& mVulkanState;
};

#endif
