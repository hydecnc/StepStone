#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

// Check Vulkan function results
#define VK_CHECK(result) if (result != VK_SUCCESS) { fprintf(stderr, "Vulkan error: %d\n", result); exit(1); }

void cleanup(VkInstance instance, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool,
             VkCommandBuffer commandBuffer, VkQueue queue, VkRenderPass renderPass, VkFramebuffer framebuffer,
             VkImage image, VkDeviceMemory memory, VkImageView imageView) {
    vkDeviceWaitIdle(device);

    if (imageView) vkDestroyImageView(device, imageView, NULL);
    if (image) vkDestroyImage(device, image, NULL);
    if (memory) vkFreeMemory(device, memory, NULL);
    if (framebuffer) vkDestroyFramebuffer(device, framebuffer, NULL);
    if (renderPass) vkDestroyRenderPass(device, renderPass, NULL);
    if (commandPool) vkDestroyCommandPool(device, commandPool, NULL);
    if (device) vkDestroyDevice(device, NULL);
    if (instance) vkDestroyInstance(instance, NULL);
}

int main() {
    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView imageView;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;

    // Create Vulkan instance
    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .pApplicationName = "Offscreen", .apiVersion = VK_API_VERSION_1_0 };
    VkInstanceCreateInfo instanceInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, .pApplicationInfo = &appInfo };
    VK_CHECK(vkCreateInstance(&instanceInfo, NULL, &instance));
    printf("create vulkan instance: %lx\n", instance);

    // Select physical device
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    VkPhysicalDevice physicalDevices[physicalDeviceCount];
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);
    physicalDevice = physicalDevices[0];
    printf("select physical device: %d\n", physicalDevice);

    // Create logical device and queue
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .queueCount = 1, .pQueuePriorities = &queuePriority };
    VkDeviceCreateInfo deviceInfo = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, .queueCreateInfoCount = 1, .pQueueCreateInfos = &queueCreateInfo };
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device));
    vkGetDeviceQueue(device, 0, 0, &queue);
    printf("create device: %d\n", device);

    // Create command pool and command buffer
    VkCommandPoolCreateInfo poolInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT };
    VK_CHECK(vkCreateCommandPool(device, &poolInfo, NULL, &commandPool));
    VkCommandBufferAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .commandPool = commandPool, .commandBufferCount = 1 };
    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));

    // Create an offscreen image
    VkImageCreateInfo imageInfo = { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, .imageType = VK_IMAGE_TYPE_2D, .format = VK_FORMAT_R8G8B8A8_UNORM,
                                     .extent = { WIDTH, HEIGHT, 1 }, .mipLevels = 1, .arrayLayers = 1, .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .tiling = VK_IMAGE_TILING_OPTIMAL, .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT };
    VK_CHECK(vkCreateImage(device, &imageInfo, NULL, &image));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);
    VkMemoryAllocateInfo allocInfoMem = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, .allocationSize = memoryRequirements.size };
    VK_CHECK(vkAllocateMemory(device, &allocInfoMem, NULL, &memory));
    VK_CHECK(vkBindImageMemory(device, image, memory, 0));

    // Create image view
    VkImageViewCreateInfo imageViewInfo = { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, .image = image, .viewType = VK_IMAGE_VIEW_TYPE_2D, .format = VK_FORMAT_R8G8B8A8_UNORM };
    VK_CHECK(vkCreateImageView(device, &imageViewInfo, NULL, &imageView));

    // Create render pass
    VkAttachmentDescription attachmentDescription = { .format = VK_FORMAT_R8G8B8A8_UNORM, .samples = VK_SAMPLE_COUNT_1_BIT,
                                                       .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                       .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, .finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };
    VkAttachmentReference attachmentRef = { .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkSubpassDescription subpass = { .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS, .colorAttachmentCount = 1, .pColorAttachments = &attachmentRef };
    VkRenderPassCreateInfo renderPassInfo = { .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, .attachmentCount = 1, .pAttachments = &attachmentDescription,
                                              .subpassCount = 1, .pSubpasses = &subpass };
    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass));

    // Create framebuffer
    VkFramebufferCreateInfo framebufferInfo = { .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, .renderPass = renderPass, .attachmentCount = 1,
                                                .pAttachments = &imageView, .width = WIDTH, .height = HEIGHT, .layers = 1 };
    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffer));

    // Record command buffer
    VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkClearValue clearColor = { .color = {{0.0f, 0.0f, 0.0f, 1.0f}} };
    VkRenderPassBeginInfo renderPassInfoBegin = { .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, .renderPass = renderPass, .framebuffer = framebuffer,
                                                  .renderArea = { .offset = {0, 0}, .extent = {WIDTH, HEIGHT} }, .clearValueCount = 1, .pClearValues = &clearColor };
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfoBegin, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &commandBuffer };
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    cleanup(instance, device, physicalDevice, commandPool, commandBuffer, queue, renderPass, framebuffer, image, memory, imageView);
    return 0;
}