workspace ("Testing raylib")
	configurations { "Debug", "Release" }
	platforms { "x64" }
	location "../"
    targetdir "../bin/%{cfg.buildcfg}"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	filter {}
	
project ("raylib-start")
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	files { "../hello_world.cpp" }
	includedirs { "../include" }
	libdirs { "../lib" }
	links { "raylib", "gdi32", "winmm" }
