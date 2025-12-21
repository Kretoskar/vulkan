workspace "FF"
	language "C++"
	architecture "x86_64"
	startproject "FFSandbox"
	configurations { "Debug", "Release" }

	filter { "configurations:Debug" }
		symbols "On"

	filter { "configurations:Release" }
		optimize "On"

	-- Reset the filter for other settings
	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

function includeGLFW()
	includedirs "vendor/GLFW/Include"
end

function linkGLFW()
	libdirs "vendor/GLFW/lib-vc2022"

	-- static lib should not link against GLFW
	filter "kind:not StaticLib"
		links "glfw3"
	filter {}
end

project "FFCore"
	location "FFCore"
	kind "StaticLib"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
	
	includedirs
	{
		"%{prj.name}/include"
	}
	
	includeGLFW()
	
	filter "configurations:Debug"
		defines "FF_DEBUG"
		runtime "Debug"
		symbols "on"

    filter "configurations:Release"
		defines "FF_RELEASE"
		runtime "Release"
		optimize "on"

project "FFEngine"
	location "FFEngine"
    kind "StaticLib"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

	includedirs
	{
		"%{prj.name}/include",
		"FFEngine/include",
		"../../VulkanSDK/1.4.335.0/Include",
	}

	links
	{
		"FFCore",
		"../../VulkanSDK/1.4.335.0/Lib/vulkan-1.lib",
		
		-- debug only
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRVd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Toolsd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Tools-diffd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Tools-optd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/OGLCompilerd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/glslangd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/OSDependentd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/GenericCodeGend.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/MachineIndependentd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/glslang-default-resource-limitsd.lib"
	}
	
	includeGLFW()
	
	dependson { "FFCore" }

	filter { "system:windows" }
    	links { "OpenGL32" }
		
	filter {}
	
		filter "configurations:Debug"
		defines "FF_DEBUG"
		runtime "Debug"
		symbols "on"

    filter "configurations:Release"
		defines "FF_RELEASE"
		runtime "Release"
		optimize "on"

project "FFSandbox"
	location "FFSandbox"
    kind "ConsoleApp"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

	includedirs
	{
		"FFCore/include",
		"FFEngine/include",
		"FFSandbox/include",
		"../../VulkanSDK/1.4.335.0/Include",
	}

	links
	{
		"FFCore",
		"FFEngine",
		"../../VulkanSDK/1.4.335.0/Lib/vulkan-1.lib",
		
		-- debug only
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRVd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Toolsd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Tools-diffd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/SPIRV-Tools-optd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/OGLCompilerd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/glslangd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/OSDependentd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/GenericCodeGend.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/MachineIndependentd.lib",
		-- "../../VulkanSDK/1.4.335.0/Lib/glslang-default-resource-limitsd.lib"
	}
	
	includeGLFW()
	linkGLFW()
	
	filter "configurations:Debug"
		defines "FF_DEBUG"
		runtime "Debug"
		symbols "on"

    filter "configurations:Release"
		defines "FF_RELEASE"
		runtime "Release"
		optimize "on"
	
	dependson { "FFCore", "FFEngine" }