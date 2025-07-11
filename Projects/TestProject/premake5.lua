
EngineLocation = "../.."

include(EngineLocation .. "/vendor/premake/solution_items.lua")

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

workspace "TestProject"
	architecture "x64"
	startproject "TestProject"
	configurations { "DebugEditor", "ReleaseEditor", "Debug", "Release" }
	solution_items { ".editorconfig" }
	flags { "MultiProcessorCompile" }
	filter "system:windows"
		buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/utf-8" }
	
	group "Dependencies"
		include(EngineLocation .. "/vendor/premake")
		include(EngineLocation .. "/vendor/glfw")
		include(EngineLocation .. "/vendor/glad")
		include(EngineLocation .. "/vendor/imgui")
		include(EngineLocation .. "/vendor/yaml-cpp")
	group ""
	
	include(EngineLocation .. "/Haste")
	include(EngineLocation .. "/HasteEditor")
	
	project "Haste"
		removedefines { "WITH_EDITOR" }

		filter "configurations:DebugEditor"
			defines { "IS_DEBUG", "WITH_EDITOR" }
			runtime "Debug"
			symbols "On"
	
		filter "configurations:ReleaseEditor"
			defines { "IS_RELEASE", "WITH_EDITOR" }
			runtime "Release"
			optimize "On"
			symbols "Off"
	
    project "HasteEditor"
        kind "None"

        filter "configurations:DebugEditor"
        	kind "StaticLib"
            defines { "IS_DEBUG", "WITH_PROJECT" }
            runtime "Debug"
            symbols "On"

        filter "configurations:ReleaseEditor"
        	kind "StaticLib"
            defines { "IS_RELEASE", "WITH_PROJECT" }
            runtime "Release"
            optimize "On"
            symbols "Off"

		
project "TestProject"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{EngineLocation}/Haste/src",
		"%{EngineLocation}/vendor/glfw/include",
		"%{EngineLocation}/vendor/glad/include",
		"%{EngineLocation}/vendor/spdlog/include",
		"%{EngineLocation}/vendor/yaml-cpp/include",
		"%{EngineLocation}/vendor/glm",
		"%{EngineLocation}/vendor/stb",
		"%{EngineLocation}/vendor/imgui",
	}
	
	defines { "WITH_PROJECT" }

	links { "Haste", "HasteEditor" }
	
	filter "system:windows"
		systemversion "latest"
		defines { "WINDOWS" }

	filter "configurations:Debug"
		defines { "IS_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:DebugEditor"
		includedirs { "%{EngineLocation}/HasteEditor/src" }
		defines { "IS_DEBUG", "WITH_EDITOR" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "IS_RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "Off"
		kind "WindowedApp"

	filter "configurations:ReleaseEditor"
		includedirs { "%{EngineLocation}/HasteEditor/src" }
		defines { "IS_RELEASE", "WITH_EDITOR" }
		runtime "Release"
		optimize "On"
		symbols "Off"
		kind "WindowedApp"

