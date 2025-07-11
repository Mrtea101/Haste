
project "HasteEditor"
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
		"../Haste/src",
		"../vendor/glfw/include",
		"../vendor/glad/include",
		"../vendor/spdlog/include",
		"../vendor/yaml-cpp/include",
		"../vendor/glm",
		"../vendor/stb",
		"../vendor/imgui",
	}

	links { "Haste" }
	
	defines { "WITH_EDITOR" }
	
	filter "system:windows"
		systemversion "latest"
		defines { "WINDOWS" }

	filter "configurations:Debug"
		defines { "IS_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "IS_RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "Off"
		kind "WindowedApp"
