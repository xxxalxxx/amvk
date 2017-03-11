#include "vulkan_image_creator.h"

VulkanImageDesc::VulkanImageDesc(const VkDevice& vkDevice):
	width(0),
	height(0),
	image(VK_NULL_HANDLE),
	imageView(VK_NULL_HANDLE),
	memory(VK_NULL_HANDLE),
	mVkDevice(vkDevice)
{

}


VulkanImageDesc::VulkanImageDesc(const VkDevice& vkDevice, uint32_t width, uint32_t height):
	width(width),
	height(height),
	image(VK_NULL_HANDLE),
	imageView(VK_NULL_HANDLE),
	memory(VK_NULL_HANDLE),
	mVkDevice(vkDevice)


{

}

VulkanImageDesc::~VulkanImageDesc() 
{
	if (imageView != VK_NULL_HANDLE)
		vkDestroyImageView(mVkDevice, imageView, nullptr);
	if (image != VK_NULL_HANDLE)
		vkDestroyImage(mVkDevice, image, nullptr);
	if (memory != VK_NULL_HANDLE)
		vkFreeMemory(mVkDevice, memory, nullptr);
}


ImageHelper::ImageHelper(const VulkanState& vulkanState):
	mVulkanState(vulkanState) 
{

}

void ImageHelper::createImage(
		uint32_t w, 
		uint32_t h, 
		VkFormat format, 
		VkImageTiling tiling,
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& imageMemory) const
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = w;
	imageInfo.extent.height = h;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_RESULT(vkCreateImage(mVulkanState.device, &imageInfo, nullptr, &image));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(mVulkanState.device, image, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = BufferHelper::getMemoryType(mVulkanState.physicalDevice, memReqs.memoryTypeBits, properties);

	VK_CHECK_RESULT(vkAllocateMemory(mVulkanState.device, &allocInfo, nullptr, &imageMemory));

	vkBindImageMemory(mVulkanState.device, image, imageMemory, 0);
}

void ImageHelper::createImage(
		const VulkanState& state,
		VulkanImageDesc& imageDesc, 
		VkFormat format, 
		VkImageTiling tiling,
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties)
{

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = imageDesc.width;
	imageInfo.extent.height = imageDesc.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_RESULT(vkCreateImage(state.device, &imageInfo, nullptr, &imageDesc.image));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(state.device, imageDesc.image, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = BufferHelper::getMemoryType(state.physicalDevice, memReqs.memoryTypeBits, properties);

	VK_CHECK_RESULT(vkAllocateMemory(state.device, &allocInfo, nullptr, &imageDesc.memory));

	vkBindImageMemory(state.device, imageDesc.image, imageDesc.memory, 0);
}


void ImageHelper::createImageView(
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		VkImageView& imageView) const
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;

	viewInfo.subresourceRange.aspectMask = aspectFlags;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK_RESULT(vkCreateImageView(mVulkanState.device, &viewInfo, nullptr, &imageView));
}


void ImageHelper::createImageView(
		const VkDevice& device,
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		VkImageView& imageView)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;

	viewInfo.subresourceRange.aspectMask = aspectFlags;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
}

void ImageHelper::createImageView(
		const VkDevice& device,
		VulkanImageDesc& imageDesc, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags)
{
	ImageHelper::createImageView(device, imageDesc.image, format, aspectFlags, imageDesc.imageView);
}

void ImageHelper::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const
{
	CmdPass cmdPass(mVulkanState.device, mVulkanState.commandPool, mVulkanState.graphicsQueue);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	} else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED 
	&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED 
	&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
	&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
	&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
							  | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
			cmdPass.commandBuffer(),
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0, 
			0, 
			nullptr, 
			0, 
			nullptr, 
			1, 
			&barrier);
}

void ImageHelper::transitionLayout(
		const VulkanState& state,
		VkImage image, 
		VkFormat format, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout,
		VkImageAspectFlags barrierAspectMask,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask)
{
	CmdPass cmdPass(state.device, state.commandPool, state.graphicsQueue);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = barrierAspectMask;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	vkCmdPipelineBarrier(
			cmdPass.commandBuffer(),
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0, 
			0, 
			nullptr, 
			0, 
			nullptr, 
			1, 
			&barrier);
}

void ImageHelper::copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height) const
{
	CmdPass cmd(mVulkanState.device, mVulkanState.commandPool, mVulkanState.graphicsQueue);

	VkImageSubresourceLayers subRes = {};
	subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRes.baseArrayLayer = 0;
	subRes.mipLevel = 0;
	subRes.layerCount = 1;

	VkImageCopy copy = {};
	copy.srcSubresource = subRes;
	copy.dstSubresource = subRes;
	copy.srcOffset = {0, 0, 0};
	copy.dstOffset = {0, 0, 0};
	copy.extent.width = width;
	copy.extent.height = height;
	copy.extent.depth = 1;

	vkCmdCopyImage(
			cmd.commandBuffer(), 
			srcImage, 
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dstImage, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, 
			&copy);
}


void ImageHelper::copyImage(const VulkanState& state, VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height)
{
	CmdPass cmd(state.device, state.commandPool, state.graphicsQueue);

	VkImageSubresourceLayers subRes = {};
	subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRes.baseArrayLayer = 0;
	subRes.mipLevel = 0;
	subRes.layerCount = 1;

	VkImageCopy copy = {};
	copy.srcSubresource = subRes;
	copy.dstSubresource = subRes;
	copy.srcOffset = {0, 0, 0};
	copy.dstOffset = {0, 0, 0};
	copy.extent.width = width;
	copy.extent.height = height;
	copy.extent.depth = 1;

	vkCmdCopyImage(
			cmd.commandBuffer(), 
			srcImage, 
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dstImage, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, 
			&copy);
}


	
void ImageHelper::copyImage(
			const VulkanState& state, 
			VulkanImageDesc& srcImage, 
			VulkanImageDesc& dstImage) 
{
	copyImage(state, srcImage.image, dstImage.image, srcImage.width, srcImage.height);
}


VkFormat ImageHelper::findDepthFormat() const
{
	return findSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat ImageHelper::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(mVulkanState.physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR
		&& (props.linearTilingFeatures & features) == features) 
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL
		&& (props.optimalTilingFeatures & features) == features)
			return format;
	}
	throw new std::runtime_error("failed to find supported format");
}


VkFormat ImageHelper::findDepthFormat(const VkPhysicalDevice& physicalDevice)
{
	return ImageHelper::findSupportedFormat(
			physicalDevice,
			{VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat ImageHelper::findSupportedFormat(
		const VkPhysicalDevice& physicalDevice, 
		const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, 
		VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR
		&& (props.linearTilingFeatures & features) == features) 
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL
		&& (props.optimalTilingFeatures & features) == features)
			return format;
	}
	throw new std::runtime_error("failed to find supported format");
}

