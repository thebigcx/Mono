workspace "Mono"
    configurations {"Debug", "Release"}

project "Mono"

	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/Mono"
	objdir "%{wks.location}/obj/%{cfg.buildcfg}/Mono"

	files {
		"src/**.cpp",
        "include/**.h"
	}
	
	includedirs {
		"%{wks.location}/include",
		"/usr/include/mono-2.0" -- TODO: fix this
	}
	
	links {
		"mono-2.0",
		"dl"
	}
	
	filter "configurations:Debug"
		buildoptions "-pg"
        runtime "Release"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
