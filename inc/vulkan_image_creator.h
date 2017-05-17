#ifndef AMVK_VULKAN_IMAGE_MANAGER_H
#define AMVK_VULKAN_IMAGE_MANAGER_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include "cmd_pass.h"
#include "vulkan_state.h"
#include "vulkan_utils.h"
#include "buffer_helper.h"
#include "vulkan_image_info.h"
#include "texture_data.h"
#include "macro.h"

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
			ImageInfo& imageDesc, 
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
			ImageInfo& srcImage, 
			ImageInfo& dstImage);

	static void copyImage(
			VkCommandBuffer& cmdBuffer, 
			VkImage srcImage, 
			VkImage dstImage, 
			uint32_t width, 
			uint32_t height);

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

	static void createImageView(
			const VkDevice& device,
			VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags, 
			VkImageView& imageView);

	static void createImageView(
			const VkDevice& device,
			ImageInfo& imageDesc, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags);
		
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

	static void transitionLayout(
		VkCommandBuffer& cmdBuffer,
		VkImage image, 
		VkFormat format, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout,
		VkImageAspectFlags barrierAspectMask,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask);

	static void createStagedImage(
		ImageInfo& imageInfo, 
		const TextureData& textureData,
		VulkanState& state,  
		const VkCommandPool& cmdPool, 
		const VkQueue& cmdQueue); 

private:
	const VulkanState& mVulkanState;
};

#endif
