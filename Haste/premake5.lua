
project "Haste"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"../vendor/glm/glm/**.hpp",
		"../vendor/glm/glm/**.inl",
	}

	includedirs
	{
		"src",
		"../vendor/glfw/include",
		"../vendor/glad/include",
		"../vendor/spdlog/include",
		"../vendor/yaml-cpp/include",
		"../vendor/glm",
		"../vendor/stb",
		"../vendor/imgui",
	}

	links
	{
		"opengl32.lib",
		"glfw",
		"glad",
		"imgui",
		"yaml-cpp",
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
		"WITH_EDITOR",
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
