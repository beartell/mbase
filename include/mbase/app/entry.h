#ifndef MBASE_ENTRY_H
#define MBASE_ENTRY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/app/ev_loop.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <Windows.h>
#include <iostream>

MBASE_BEGIN

LONG_PTR oldAddr = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        std::cout << "We are here!" << std::endl;
        return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

class app {
public:
    struct QueueFamilyIndices {
        U32 graphicsFamily;
        U32 presentFamily;
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR* formats;
        VkPresentModeKHR* presentModes;
    };

	app() {
		GetStartupInfo(&winInfo);
	}

    ~app() {
        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroySurfaceKHR(vki, surface, nullptr);
        vkDestroyInstance(vki, nullptr);
    }

	GENERIC run() {
        // Register the window class.
        const char CLASS_NAME[] = "Sample Window Class";

        WNDCLASSA wc = { };
        
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        // Create the window.

        U32 tempWidth = 1280;
        U32 tempHeight = 768;

        HWND hwnd = CreateWindowExA(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            "Learn to Program Vulkan",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, tempWidth, tempHeight,

            NULL,       // Parent window    
            NULL,       // Menu
            GetModuleHandle(NULL),  // Instance handle
            NULL        // Additional application data
        );

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        
        LARGE_INTEGER queryTime;
        MSG msg = { };
        LARGE_INTEGER performanceFrequency = {};
        QueryPerformanceFrequency(&performanceFrequency);
        U32 mFrequency = performanceFrequency.QuadPart;

        U32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        VkExtensionProperties* vkProps = new VkExtensionProperties[extensionCount]; // MEMORY ALLOCATION
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkProps);
        mbase::vector<const char*> extensionList;
        for(U32 i = 0; i < extensionCount; i++)
        {
            extensionList.push_back(vkProps[i].extensionName);
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Vulkan!";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo vkiInfo{};
        vkiInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkiInfo.pApplicationInfo = &appInfo;
        vkiInfo.enabledExtensionCount = extensionCount;
        vkiInfo.ppEnabledExtensionNames = extensionList.data();
        
        for(U32 i = 0; i < extensionCount; i++)
        {
            std::cout << extensionList[i] << std::endl;
        }
        
        VkResult vr = vkCreateInstance(&vkiInfo, nullptr, &vki);
        if(vr != VK_SUCCESS)
        {
            std::cout << "Vr failed: " << vr << std::endl;
        }

        U32 deviceCount = 0;
        vkEnumeratePhysicalDevices(vki, &deviceCount, nullptr);
        vkEnumeratePhysicalDevices(vki, &deviceCount, &physicalDevice);
        
        F32 queuePriority = 1.0f;

        QueueFamilyIndices qfi = FindQueueFamilies();

        VkDeviceQueueCreateInfo queueCreationInfo{};
        queueCreationInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreationInfo.queueFamilyIndex = qfi.graphicsFamily;
        queueCreationInfo.queueCount = 1;
        queueCreationInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures logicalDeviceFeatures{};

        const char* bs = VK_KHR_SWAPCHAIN_EXTENSION_NAME; // BS is for BULLSHIT

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreationInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &logicalDeviceFeatures;
        createInfo.enabledExtensionCount = 1;
        createInfo.ppEnabledExtensionNames = &bs;

        vr = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
        std::cout << vr << std::endl;
        
        vkGetDeviceQueue(logicalDevice, qfi.graphicsFamily, 0, &graphicsQueue);
        
        VkWin32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = hwnd;
        surfaceInfo.hinstance = GetModuleHandle(nullptr);
        
        vr = vkCreateWin32SurfaceKHR(vki, &surfaceInfo, nullptr, &surface);
        std::cout << "Surface creation result: " << vr << std::endl;

        std::cout << "Is supported: " << IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME) << std::endl;

        QuerySwapChainSupport();

        VkSurfaceFormatKHR m_SurfaceFormat = GetSurfaceFormat();
        VkPresentModeKHR m_PresentMode = GetPresentMode();
        VkExtent2D m_Extent = GetSwapExtent();

        VkSwapchainCreateInfoKHR swapCreateInfo{};

        swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapCreateInfo.surface = surface;
        swapCreateInfo.minImageCount = details.capabilities.minImageCount + 1;
        swapCreateInfo.imageFormat = m_SurfaceFormat.format;
        swapCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapCreateInfo.imageExtent = m_Extent;
        swapCreateInfo.imageArrayLayers = 1;
        swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0;
        swapCreateInfo.pQueueFamilyIndices = nullptr;
        swapCreateInfo.preTransform = details.capabilities.currentTransform;
        swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapCreateInfo.presentMode = m_PresentMode;
        swapCreateInfo.clipped = VK_TRUE;
       
        vr = vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, nullptr, &swapChain);
        std::cout << "Swap chain init result: " << vr << std::endl;

        U32 imageCount = 0;
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
        swapChainImages = new VkImage[imageCount];
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages);
        
        VkImageView* m_ImageViews = new VkImageView[imageCount];
        for(U32 i = 0 ; i < imageCount; i++)
        {
            VkImageViewCreateInfo imgViewInfo{};
            imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imgViewInfo.image = swapChainImages[i];
            imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imgViewInfo.format = m_SurfaceFormat.format;
            imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            
            imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imgViewInfo.subresourceRange.baseMipLevel = 0;
            imgViewInfo.subresourceRange.levelCount = 1;
            imgViewInfo.subresourceRange.baseArrayLayer = 0;
            imgViewInfo.subresourceRange.layerCount = 1;
            
            vkCreateImageView(logicalDevice, &imgViewInfo, nullptr, m_ImageViews + i);
        }

        mbase::deep_char_stream myVertStream = std::move(GetShaderBytecode("vert.spv"));
        mbase::deep_char_stream myFragStream = std::move(GetShaderBytecode("frag.spv"));

        std::cout << "Vert length: " << myVertStream.buffer_length() << std::endl;
        std::cout << "Frag length: " << myFragStream.buffer_length() << std::endl;

        while (msg.message != WM_QUIT)
        {
            QueryPerformanceCounter(&queryTime);

            U32 startTime = queryTime.QuadPart;
            Sleep(15);

            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            QueryPerformanceCounter(&queryTime);
            U32 endTime = queryTime.QuadPart;
            U32 elapsedTime = ((endTime - startTime) * 1000) / mFrequency;
            eventLoop.ManualRunTimers(elapsedTime);
            float abc[4] = {1, 0, 0, 1};
        }
	}

    mbase::deep_char_stream GetShaderBytecode(const mbase::string& in_fileName) {
        mbase::io_file fileIo(in_fileName, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::OPEN);
        U32 fileSize = fileIo.get_file_size();
        mbase::deep_char_stream myCharStream(fileSize);
        fileIo.read_data(myCharStream);
        fileIo.close_file();
        return myCharStream;
    }

    bool IsDeviceSuitable() {
        if(physicalDevice == VK_NULL_HANDLE)
        {
            return false;
        }
        
        VkPhysicalDeviceProperties deviceProps;
        
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        return deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    }

    QueueFamilyIndices FindQueueFamilies() {
        QueueFamilyIndices indicies;
        indicies.graphicsFamily = 0;
        indicies.presentFamily = 0;
        U32 queueFamilyCount = 0;
        
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        VkQueueFamilyProperties* vqf = new VkQueueFamilyProperties[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, vqf);
        
        for(U32 i = 0; i < queueFamilyCount; i++)
        {
            if (vqf[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indicies.graphicsFamily = i;
            }
            i++;
        }

        /*VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, indicies.graphicsFamily, surface, &presentSupport);

        if(presentSupport)
        {
            indicies.presentFamily = indicies.graphicsFamily;
        }*/
        indicies.presentFamily = indicies.graphicsFamily;
        delete[] vqf;
        return indicies;
    }

    bool IsExtensionSupported(const mbase::string& in_extension) {
        // CODES ARE MESSY FOR PROTOTYPING
        U32 extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        VkExtensionProperties* exProps = new VkExtensionProperties[extensionCount];
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, exProps);
        for(U32 i = 0; i < extensionCount; i++)
        {
            mbase::string ourExtension = exProps[i].extensionName;
            if(ourExtension == in_extension)
            {
                delete[] exProps;
                return true;
            }
        }
        delete[] exProps;
        return false;
    }

    SwapChainSupportDetails* QuerySwapChainSupport() {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
        
        U32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if(formatCount)
        {
            // MEMORY LEAK HERE BUT DON'T MIND FOR NOW
            details.formats = new VkSurfaceFormatKHR[formatCount];
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats);
        }
        
        U32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        if(presentModeCount)
        {
            details.presentModes = new VkPresentModeKHR[presentModeCount];
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes);
        }

        return &details;
    }

    VkSurfaceFormatKHR GetSurfaceFormat() {
        return { VK_FORMAT_B8G8R8A8_SRGB ,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    VkPresentModeKHR GetPresentMode() {
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D GetSwapExtent() {
        return { details.capabilities.minImageExtent.width, details.capabilities.minImageExtent.height };
    }

    ev_loop* GetApplicationLoop() {
        return &eventLoop;
    }

private:
    VkPhysicalDeviceFeatures deviceFeatures;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkInstance vki = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkImage* swapChainImages = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;

    SwapChainSupportDetails details;

    mbase::wstring applicationName;
	STARTUPINFO winInfo;
	ev_loop eventLoop;
};

MBASE_END

#endif // MBASE_ENTRY_H