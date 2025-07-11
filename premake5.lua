
include "vendor/premake/solution_items.lua"

workspace "Haste"
	architecture "x64"
	startproject "HasteEditor"
	configurations { "Debug", "Release" }
	solution_items { ".editorconfig" }
	flags { "MultiProcessorCompile" }
	filter "system:windows"
		buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/utf-8" }

outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
	include "vendor/premake"
	include "vendor/glfw"
	include "vendor/glad"
	include "vendor/imgui"
	include "vendor/yaml-cpp"
group ""

include "Haste"
include "HasteEditor"
