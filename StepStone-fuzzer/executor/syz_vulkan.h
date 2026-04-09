#include <vulkan/vulkan.h>

#define VULKAN_API(ret, func, args_with_types, args) \
	static ret syz_##func args_with_types        \
	{                                            \
		return func args;                    \
	}

#define EMPTY_VULKAN_API(ret, func, args_with_types, args) \
	static void syz_##func(void)                       \
	{                                                  \
		return;                                    \
	}

static void loop();

#if SYZ_EXECUTOR || __NR_syz_vkCreateInstance
VULKAN_API(VkResult, vkCreateInstance, (const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance), (pCreateInfo, pAllocator, pInstance))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyInstance
VULKAN_API(void, vkDestroyInstance, (VkInstance instance, const VkAllocationCallbacks* pAllocator), (instance, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEnumeratePhysicalDevices
VULKAN_API(VkResult, vkEnumeratePhysicalDevices, (VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices), (instance, pPhysicalDeviceCount, pPhysicalDevices))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceFeatures
VULKAN_API(void, vkGetPhysicalDeviceFeatures, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures), (physicalDevice, pFeatures))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceFormatProperties
VULKAN_API(void, vkGetPhysicalDeviceFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties), (physicalDevice, format, pFormatProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceImageFormatProperties
VULKAN_API(VkResult, vkGetPhysicalDeviceImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties), (physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceProperties
VULKAN_API(void, vkGetPhysicalDeviceProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties), (physicalDevice, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceQueueFamilyProperties
VULKAN_API(void, vkGetPhysicalDeviceQueueFamilyProperties, (VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties), (physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceMemoryProperties
VULKAN_API(void, vkGetPhysicalDeviceMemoryProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties), (physicalDevice, pMemoryProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetInstanceProcAddr
VULKAN_API(PFN_vkVoidFunction, vkGetInstanceProcAddr, (VkInstance instance, const char* pName), (instance, pName))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceProcAddr
VULKAN_API(PFN_vkVoidFunction, vkGetDeviceProcAddr, (VkDevice device, const char* pName), (device, pName))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateDevice
VULKAN_API(VkResult, vkCreateDevice, (VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice), (physicalDevice, pCreateInfo, pAllocator, pDevice))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyDevice
VULKAN_API(void, vkDestroyDevice, (VkDevice device, const VkAllocationCallbacks* pAllocator), (device, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEnumerateInstanceExtensionProperties
VULKAN_API(VkResult, vkEnumerateInstanceExtensionProperties, (const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), (pLayerName, pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEnumerateDeviceExtensionProperties
VULKAN_API(VkResult, vkEnumerateDeviceExtensionProperties, (VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), (physicalDevice, pLayerName, pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEnumerateInstanceLayerProperties
VULKAN_API(VkResult, vkEnumerateInstanceLayerProperties, (uint32_t * pPropertyCount, VkLayerProperties* pProperties), (pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEnumerateDeviceLayerProperties
VULKAN_API(VkResult, vkEnumerateDeviceLayerProperties, (VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties), (physicalDevice, pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceQueue
VULKAN_API(void, vkGetDeviceQueue, (VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue), (device, queueFamilyIndex, queueIndex, pQueue))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkQueueSubmit
VULKAN_API(VkResult, vkQueueSubmit, (VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence), (queue, submitCount, pSubmits, fence))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkQueueWaitIdle
VULKAN_API(VkResult, vkQueueWaitIdle, (VkQueue queue), (queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDeviceWaitIdle
VULKAN_API(VkResult, vkDeviceWaitIdle, (VkDevice device), (device))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkAllocateMemory
VULKAN_API(VkResult, vkAllocateMemory, (VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory), (device, pAllocateInfo, pAllocator, pMemory))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkFreeMemory
VULKAN_API(void, vkFreeMemory, (VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator), (device, memory, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkMapMemory
VULKAN_API(VkResult, vkMapMemory, (VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData), (device, memory, offset, size, flags, ppData))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkUnmapMemory
VULKAN_API(void, vkUnmapMemory, (VkDevice device, VkDeviceMemory memory), (device, memory))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkFlushMappedMemoryRanges
VULKAN_API(VkResult, vkFlushMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), (device, memoryRangeCount, pMemoryRanges))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkInvalidateMappedMemoryRanges
VULKAN_API(VkResult, vkInvalidateMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), (device, memoryRangeCount, pMemoryRanges))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceMemoryCommitment
VULKAN_API(void, vkGetDeviceMemoryCommitment, (VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes), (device, memory, pCommittedMemoryInBytes))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkBindBufferMemory
VULKAN_API(VkResult, vkBindBufferMemory, (VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset), (device, buffer, memory, memoryOffset))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkBindImageMemory
VULKAN_API(VkResult, vkBindImageMemory, (VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset), (device, image, memory, memoryOffset))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetBufferMemoryRequirements
VULKAN_API(void, vkGetBufferMemoryRequirements, (VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements), (device, buffer, pMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetImageMemoryRequirements
VULKAN_API(void, vkGetImageMemoryRequirements, (VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements), (device, image, pMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetImageSparseMemoryRequirements
VULKAN_API(void, vkGetImageSparseMemoryRequirements, (VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements), (device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceSparseImageFormatProperties
VULKAN_API(void, vkGetPhysicalDeviceSparseImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties), (physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkQueueBindSparse
VULKAN_API(VkResult, vkQueueBindSparse, (VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence), (queue, bindInfoCount, pBindInfo, fence))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateFence
VULKAN_API(VkResult, vkCreateFence, (VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence), (device, pCreateInfo, pAllocator, pFence))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyFence
VULKAN_API(void, vkDestroyFence, (VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator), (device, fence, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetFences
VULKAN_API(VkResult, vkResetFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences), (device, fenceCount, pFences))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetFenceStatus
VULKAN_API(VkResult, vkGetFenceStatus, (VkDevice device, VkFence fence), (device, fence))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkWaitForFences
VULKAN_API(VkResult, vkWaitForFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout), (device, fenceCount, pFences, waitAll, timeout))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateSemaphore
VULKAN_API(VkResult, vkCreateSemaphore, (VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore), (device, pCreateInfo, pAllocator, pSemaphore))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroySemaphore
VULKAN_API(void, vkDestroySemaphore, (VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator), (device, semaphore, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateEvent
VULKAN_API(VkResult, vkCreateEvent, (VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent), (device, pCreateInfo, pAllocator, pEvent))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyEvent
VULKAN_API(void, vkDestroyEvent, (VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator), (device, event, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetEventStatus
VULKAN_API(VkResult, vkGetEventStatus, (VkDevice device, VkEvent event), (device, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkSetEvent
VULKAN_API(VkResult, vkSetEvent, (VkDevice device, VkEvent event), (device, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetEvent
VULKAN_API(VkResult, vkResetEvent, (VkDevice device, VkEvent event), (device, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateQueryPool
VULKAN_API(VkResult, vkCreateQueryPool, (VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool), (device, pCreateInfo, pAllocator, pQueryPool))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyQueryPool
VULKAN_API(void, vkDestroyQueryPool, (VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator), (device, queryPool, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetQueryPoolResults
VULKAN_API(VkResult, vkGetQueryPoolResults, (VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags), (device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateBuffer
VULKAN_API(VkResult, vkCreateBuffer, (VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer), (device, pCreateInfo, pAllocator, pBuffer))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyBuffer
VULKAN_API(void, vkDestroyBuffer, (VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator), (device, buffer, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateBufferView
VULKAN_API(VkResult, vkCreateBufferView, (VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView), (device, pCreateInfo, pAllocator, pView))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyBufferView
VULKAN_API(void, vkDestroyBufferView, (VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator), (device, bufferView, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateImage
VULKAN_API(VkResult, vkCreateImage, (VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage), (device, pCreateInfo, pAllocator, pImage))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyImage
VULKAN_API(void, vkDestroyImage, (VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator), (device, image, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetImageSubresourceLayout
VULKAN_API(void, vkGetImageSubresourceLayout, (VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout), (device, image, pSubresource, pLayout))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateImageView
VULKAN_API(VkResult, vkCreateImageView, (VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView), (device, pCreateInfo, pAllocator, pView))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyImageView
VULKAN_API(void, vkDestroyImageView, (VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator), (device, imageView, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateShaderModule
VULKAN_API(VkResult, vkCreateShaderModule, (VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule), (device, pCreateInfo, pAllocator, pShaderModule))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyShaderModule
VULKAN_API(void, vkDestroyShaderModule, (VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator), (device, shaderModule, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreatePipelineCache
VULKAN_API(VkResult, vkCreatePipelineCache, (VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache), (device, pCreateInfo, pAllocator, pPipelineCache))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyPipelineCache
VULKAN_API(void, vkDestroyPipelineCache, (VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator), (device, pipelineCache, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPipelineCacheData
VULKAN_API(VkResult, vkGetPipelineCacheData, (VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData), (device, pipelineCache, pDataSize, pData))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkMergePipelineCaches
VULKAN_API(VkResult, vkMergePipelineCaches, (VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches), (device, dstCache, srcCacheCount, pSrcCaches))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateGraphicsPipelines
VULKAN_API(VkResult, vkCreateGraphicsPipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), (device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateComputePipelines
VULKAN_API(VkResult, vkCreateComputePipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), (device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyPipeline
VULKAN_API(void, vkDestroyPipeline, (VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator), (device, pipeline, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreatePipelineLayout
VULKAN_API(VkResult, vkCreatePipelineLayout, (VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout), (device, pCreateInfo, pAllocator, pPipelineLayout))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyPipelineLayout
VULKAN_API(void, vkDestroyPipelineLayout, (VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator), (device, pipelineLayout, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateSampler
VULKAN_API(VkResult, vkCreateSampler, (VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler), (device, pCreateInfo, pAllocator, pSampler))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroySampler
VULKAN_API(void, vkDestroySampler, (VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator), (device, sampler, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateDescriptorSetLayout
VULKAN_API(VkResult, vkCreateDescriptorSetLayout, (VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout), (device, pCreateInfo, pAllocator, pSetLayout))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyDescriptorSetLayout
VULKAN_API(void, vkDestroyDescriptorSetLayout, (VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator), (device, descriptorSetLayout, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateDescriptorPool
VULKAN_API(VkResult, vkCreateDescriptorPool, (VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool), (device, pCreateInfo, pAllocator, pDescriptorPool))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyDescriptorPool
VULKAN_API(void, vkDestroyDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator), (device, descriptorPool, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetDescriptorPool
VULKAN_API(VkResult, vkResetDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags), (device, descriptorPool, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkAllocateDescriptorSets
VULKAN_API(VkResult, vkAllocateDescriptorSets, (VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets), (device, pAllocateInfo, pDescriptorSets))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkFreeDescriptorSets
VULKAN_API(VkResult, vkFreeDescriptorSets, (VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets), (device, descriptorPool, descriptorSetCount, pDescriptorSets))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkUpdateDescriptorSets
VULKAN_API(void, vkUpdateDescriptorSets, (VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies), (device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateFramebuffer
VULKAN_API(VkResult, vkCreateFramebuffer, (VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer), (device, pCreateInfo, pAllocator, pFramebuffer))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyFramebuffer
VULKAN_API(void, vkDestroyFramebuffer, (VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator), (device, framebuffer, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateRenderPass
VULKAN_API(VkResult, vkCreateRenderPass, (VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass), (device, pCreateInfo, pAllocator, pRenderPass))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyRenderPass
VULKAN_API(void, vkDestroyRenderPass, (VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator), (device, renderPass, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetRenderAreaGranularity
VULKAN_API(void, vkGetRenderAreaGranularity, (VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity), (device, renderPass, pGranularity))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateCommandPool
VULKAN_API(VkResult, vkCreateCommandPool, (VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool), (device, pCreateInfo, pAllocator, pCommandPool))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyCommandPool
VULKAN_API(void, vkDestroyCommandPool, (VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator), (device, commandPool, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetCommandPool
VULKAN_API(VkResult, vkResetCommandPool, (VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags), (device, commandPool, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkAllocateCommandBuffers
VULKAN_API(VkResult, vkAllocateCommandBuffers, (VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers), (device, pAllocateInfo, pCommandBuffers))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkFreeCommandBuffers
VULKAN_API(void, vkFreeCommandBuffers, (VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers), (device, commandPool, commandBufferCount, pCommandBuffers))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkBeginCommandBuffer
VULKAN_API(VkResult, vkBeginCommandBuffer, (VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo), (commandBuffer, pBeginInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkEndCommandBuffer
VULKAN_API(VkResult, vkEndCommandBuffer, (VkCommandBuffer commandBuffer), (commandBuffer))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetCommandBuffer
VULKAN_API(VkResult, vkResetCommandBuffer, (VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags), (commandBuffer, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBindPipeline
VULKAN_API(void, vkCmdBindPipeline, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline), (commandBuffer, pipelineBindPoint, pipeline))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetViewport
VULKAN_API(void, vkCmdSetViewport, (VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports), (commandBuffer, firstViewport, viewportCount, pViewports))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetScissor
VULKAN_API(void, vkCmdSetScissor, (VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors), (commandBuffer, firstScissor, scissorCount, pScissors))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetLineWidth
VULKAN_API(void, vkCmdSetLineWidth, (VkCommandBuffer commandBuffer, float lineWidth), (commandBuffer, lineWidth))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthBias
VULKAN_API(void, vkCmdSetDepthBias, (VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor), (commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetBlendConstants
VULKAN_API(void, vkCmdSetBlendConstants, (VkCommandBuffer commandBuffer, const float blendConstants[4]), (commandBuffer, blendConstants))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthBounds
VULKAN_API(void, vkCmdSetDepthBounds, (VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds), (commandBuffer, minDepthBounds, maxDepthBounds))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetStencilCompareMask
VULKAN_API(void, vkCmdSetStencilCompareMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask), (commandBuffer, faceMask, compareMask))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetStencilWriteMask
VULKAN_API(void, vkCmdSetStencilWriteMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask), (commandBuffer, faceMask, writeMask))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetStencilReference
VULKAN_API(void, vkCmdSetStencilReference, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference), (commandBuffer, faceMask, reference))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBindDescriptorSets
VULKAN_API(void, vkCmdBindDescriptorSets, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets), (commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBindIndexBuffer
VULKAN_API(void, vkCmdBindIndexBuffer, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType), (commandBuffer, buffer, offset, indexType))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBindVertexBuffers
VULKAN_API(void, vkCmdBindVertexBuffers, (VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets), (commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDraw
VULKAN_API(void, vkCmdDraw, (VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance), (commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDrawIndexed
VULKAN_API(void, vkCmdDrawIndexed, (VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance), (commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDrawIndirect
VULKAN_API(void, vkCmdDrawIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), (commandBuffer, buffer, offset, drawCount, stride))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDrawIndexedIndirect
VULKAN_API(void, vkCmdDrawIndexedIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), (commandBuffer, buffer, offset, drawCount, stride))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDispatch
VULKAN_API(void, vkCmdDispatch, (VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ), (commandBuffer, groupCountX, groupCountY, groupCountZ))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDispatchIndirect
VULKAN_API(void, vkCmdDispatchIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset), (commandBuffer, buffer, offset))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyBuffer
VULKAN_API(void, vkCmdCopyBuffer, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions), (commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyImage
VULKAN_API(void, vkCmdCopyImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions), (commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBlitImage
VULKAN_API(void, vkCmdBlitImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter), (commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyBufferToImage
VULKAN_API(void, vkCmdCopyBufferToImage, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions), (commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetImageSparseMemoryRequirements2
VULKAN_API(void, vkGetImageSparseMemoryRequirements2, (VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements), (device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceFeatures2
VULKAN_API(void, vkGetPhysicalDeviceFeatures2, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures), (physicalDevice, pFeatures))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceProperties2
VULKAN_API(void, vkGetPhysicalDeviceProperties2, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties), (physicalDevice, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceFormatProperties2
VULKAN_API(void, vkGetPhysicalDeviceFormatProperties2, (VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties), (physicalDevice, format, pFormatProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceImageFormatProperties2
VULKAN_API(VkResult, vkGetPhysicalDeviceImageFormatProperties2, (VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties), (physicalDevice, pImageFormatInfo, pImageFormatProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceQueueFamilyProperties2
VULKAN_API(void, vkGetPhysicalDeviceQueueFamilyProperties2, (VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties), (physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceMemoryProperties2
VULKAN_API(void, vkGetPhysicalDeviceMemoryProperties2, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties), (physicalDevice, pMemoryProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceSparseImageFormatProperties2
VULKAN_API(void, vkGetPhysicalDeviceSparseImageFormatProperties2, (VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties), (physicalDevice, pFormatInfo, pPropertyCount, pProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkTrimCommandPool
VULKAN_API(void, vkTrimCommandPool, (VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags), (device, commandPool, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceQueue2
VULKAN_API(void, vkGetDeviceQueue2, (VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue), (device, pQueueInfo, pQueue))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateSamplerYcbcrConversion
VULKAN_API(VkResult, vkCreateSamplerYcbcrConversion, (VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion), (device, pCreateInfo, pAllocator, pYcbcrConversion))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroySamplerYcbcrConversion
VULKAN_API(void, vkDestroySamplerYcbcrConversion, (VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator), (device, ycbcrConversion, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateDescriptorUpdateTemplate
VULKAN_API(VkResult, vkCreateDescriptorUpdateTemplate, (VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate), (device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyDescriptorUpdateTemplate
VULKAN_API(void, vkDestroyDescriptorUpdateTemplate, (VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator), (device, descriptorUpdateTemplate, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkUpdateDescriptorSetWithTemplate
VULKAN_API(void, vkUpdateDescriptorSetWithTemplate, (VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData), (device, descriptorSet, descriptorUpdateTemplate, pData))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceExternalBufferProperties
VULKAN_API(void, vkGetPhysicalDeviceExternalBufferProperties, (VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties), (physicalDevice, pExternalBufferInfo, pExternalBufferProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceExternalFenceProperties
VULKAN_API(void, vkGetPhysicalDeviceExternalFenceProperties, (VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties), (physicalDevice, pExternalFenceInfo, pExternalFenceProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceExternalSemaphoreProperties
VULKAN_API(void, vkGetPhysicalDeviceExternalSemaphoreProperties, (VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties), (physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDescriptorSetLayoutSupport
VULKAN_API(void, vkGetDescriptorSetLayoutSupport, (VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport), (device, pCreateInfo, pSupport))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDrawIndirectCount
VULKAN_API(void, vkCmdDrawIndirectCount, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride), (commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdDrawIndexedIndirectCount
VULKAN_API(void, vkCmdDrawIndexedIndirectCount, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride), (commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreateRenderPass2
VULKAN_API(VkResult, vkCreateRenderPass2, (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass), (device, pCreateInfo, pAllocator, pRenderPass))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBeginRenderPass2
VULKAN_API(void, vkCmdBeginRenderPass2, (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo), (commandBuffer, pRenderPassBegin, pSubpassBeginInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdNextSubpass2
VULKAN_API(void, vkCmdNextSubpass2, (VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo), (commandBuffer, pSubpassBeginInfo, pSubpassEndInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdEndRenderPass2
VULKAN_API(void, vkCmdEndRenderPass2, (VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo), (commandBuffer, pSubpassEndInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkResetQueryPool
VULKAN_API(void, vkResetQueryPool, (VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount), (device, queryPool, firstQuery, queryCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetSemaphoreCounterValue
VULKAN_API(VkResult, vkGetSemaphoreCounterValue, (VkDevice device, VkSemaphore semaphore, uint64_t* pValue), (device, semaphore, pValue))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkWaitSemaphores
VULKAN_API(VkResult, vkWaitSemaphores, (VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout), (device, pWaitInfo, timeout))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkSignalSemaphore
VULKAN_API(VkResult, vkSignalSemaphore, (VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo), (device, pSignalInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetBufferDeviceAddress
VULKAN_API(VkDeviceAddress, vkGetBufferDeviceAddress, (VkDevice device, const VkBufferDeviceAddressInfo* pInfo), (device, pInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetBufferOpaqueCaptureAddress
VULKAN_API(uint64_t, vkGetBufferOpaqueCaptureAddress, (VkDevice device, const VkBufferDeviceAddressInfo* pInfo), (device, pInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceMemoryOpaqueCaptureAddress
VULKAN_API(uint64_t, vkGetDeviceMemoryOpaqueCaptureAddress, (VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo), (device, pInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPhysicalDeviceToolProperties
VULKAN_API(VkResult, vkGetPhysicalDeviceToolProperties, (VkPhysicalDevice physicalDevice, uint32_t* pToolCount, VkPhysicalDeviceToolProperties* pToolProperties), (physicalDevice, pToolCount, pToolProperties))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCreatePrivateDataSlot
VULKAN_API(VkResult, vkCreatePrivateDataSlot, (VkDevice device, const VkPrivateDataSlotCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPrivateDataSlot* pPrivateDataSlot), (device, pCreateInfo, pAllocator, pPrivateDataSlot))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkDestroyPrivateDataSlot
VULKAN_API(void, vkDestroyPrivateDataSlot, (VkDevice device, VkPrivateDataSlot privateDataSlot, const VkAllocationCallbacks* pAllocator), (device, privateDataSlot, pAllocator))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkSetPrivateData
VULKAN_API(VkResult, vkSetPrivateData, (VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data), (device, objectType, objectHandle, privateDataSlot, data))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetPrivateData
VULKAN_API(void, vkGetPrivateData, (VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t* pData), (device, objectType, objectHandle, privateDataSlot, pData))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetEvent2
VULKAN_API(void, vkCmdSetEvent2, (VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo), (commandBuffer, event, pDependencyInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdResetEvent2
VULKAN_API(void, vkCmdResetEvent2, (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask), (commandBuffer, event, stageMask))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdWaitEvents2
VULKAN_API(void, vkCmdWaitEvents2, (VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos), (commandBuffer, eventCount, pEvents, pDependencyInfos))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdPipelineBarrier2
VULKAN_API(void, vkCmdPipelineBarrier2, (VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo), (commandBuffer, pDependencyInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdWriteTimestamp2
VULKAN_API(void, vkCmdWriteTimestamp2, (VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query), (commandBuffer, stage, queryPool, query))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkQueueSubmit2
VULKAN_API(VkResult, vkQueueSubmit2, (VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence), (queue, submitCount, pSubmits, fence))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyBuffer2
VULKAN_API(void, vkCmdCopyBuffer2, (VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo), (commandBuffer, pCopyBufferInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyImage2
VULKAN_API(void, vkCmdCopyImage2, (VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo), (commandBuffer, pCopyImageInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyBufferToImage2
VULKAN_API(void, vkCmdCopyBufferToImage2, (VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo), (commandBuffer, pCopyBufferToImageInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdCopyImageToBuffer2
VULKAN_API(void, vkCmdCopyImageToBuffer2, (VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo), (commandBuffer, pCopyImageToBufferInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBlitImage2
VULKAN_API(void, vkCmdBlitImage2, (VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo), (commandBuffer, pBlitImageInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdResolveImage2
VULKAN_API(void, vkCmdResolveImage2, (VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo), (commandBuffer, pResolveImageInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBeginRendering
VULKAN_API(void, vkCmdBeginRendering, (VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo), (commandBuffer, pRenderingInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdEndRendering
VULKAN_API(void, vkCmdEndRendering, (VkCommandBuffer commandBuffer), (commandBuffer))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetCullMode
VULKAN_API(void, vkCmdSetCullMode, (VkCommandBuffer commandBuffer, VkCullModeFlags cullMode), (commandBuffer, cullMode))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetFrontFace
VULKAN_API(void, vkCmdSetFrontFace, (VkCommandBuffer commandBuffer, VkFrontFace frontFace), (commandBuffer, frontFace))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetPrimitiveTopology
VULKAN_API(void, vkCmdSetPrimitiveTopology, (VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology), (commandBuffer, primitiveTopology))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetViewportWithCount
VULKAN_API(void, vkCmdSetViewportWithCount, (VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports), (commandBuffer, viewportCount, pViewports))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetScissorWithCount
VULKAN_API(void, vkCmdSetScissorWithCount, (VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors), (commandBuffer, scissorCount, pScissors))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdBindVertexBuffers2
VULKAN_API(void, vkCmdBindVertexBuffers2, (VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides), (commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthTestEnable
VULKAN_API(void, vkCmdSetDepthTestEnable, (VkCommandBuffer commandBuffer, VkBool32 depthTestEnable), (commandBuffer, depthTestEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthWriteEnable
VULKAN_API(void, vkCmdSetDepthWriteEnable, (VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable), (commandBuffer, depthWriteEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthCompareOp
VULKAN_API(void, vkCmdSetDepthCompareOp, (VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp), (commandBuffer, depthCompareOp))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthBoundsTestEnable
VULKAN_API(void, vkCmdSetDepthBoundsTestEnable, (VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable), (commandBuffer, depthBoundsTestEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetStencilTestEnable
VULKAN_API(void, vkCmdSetStencilTestEnable, (VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable), (commandBuffer, stencilTestEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetStencilOp
VULKAN_API(void, vkCmdSetStencilOp, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp), (commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetRasterizerDiscardEnable
VULKAN_API(void, vkCmdSetRasterizerDiscardEnable, (VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable), (commandBuffer, rasterizerDiscardEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetDepthBiasEnable
VULKAN_API(void, vkCmdSetDepthBiasEnable, (VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable), (commandBuffer, depthBiasEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkCmdSetPrimitiveRestartEnable
VULKAN_API(void, vkCmdSetPrimitiveRestartEnable, (VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable), (commandBuffer, primitiveRestartEnable))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceBufferMemoryRequirements
VULKAN_API(void, vkGetDeviceBufferMemoryRequirements, (VkDevice device, const VkDeviceBufferMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements), (device, pInfo, pMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceImageMemoryRequirements
VULKAN_API(void, vkGetDeviceImageMemoryRequirements, (VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements), (device, pInfo, pMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkGetDeviceImageSparseMemoryRequirements
VULKAN_API(void, vkGetDeviceImageSparseMemoryRequirements, (VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements), (device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements))
#endif

#if SYZ_EXECUTOR || __NR_syz_vkBindBufferMemory2
VULKAN_API(VkResult, vkBindBufferMemory2, (VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos), (device, bindInfoCount, pBindInfos))
#endif