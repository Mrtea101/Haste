
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
		"%{prj.location}/src",
		"%{wks.location}/Haste/src",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/glad/include",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/stb",
		"%{wks.location}/vendor/imgui",
	}

	links
	{
		"Haste",
	}
	
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