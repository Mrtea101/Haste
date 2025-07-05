
project "Haste"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp",
		"%{wks.location}/vendor/glm/glm/**.hpp",
		"%{wks.location}/vendor/glm/glm/**.inl",
		"%{wks.location}/vendor/glm/glm/**.inl",
		"%{wks.location}/vendor/stb/stb_image.h",
	}

	includedirs
	{
		"%{prj.location}/src",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/glad/include",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/stb",
		"%{wks.location}/vendor/imgui",
	}

	links
	{
		"opengl32.lib",
		"glfw",
		"glad",
		"imgui",
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
	}
	
	filter "system:windows"
		systemversion "latest"
		defines { "WINDOWS" }
		links
		{
			"Ws2_32.lib",	--WinSock
			"Winmm.lib",	--WinMM
			"Version.lib",	--WinVersion
			"Bcrypt.lib",	--BCrypt
			"Dwmapi.lib",	--Dwmapi
		}

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