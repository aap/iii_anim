workspace "iii_anim"
	configurations { "Release","Debug" }
	location "build"

	files { "src/*.*" }
	files { "src/xmllib/*.*" }

	includedirs { "src" }
	includedirs { os.getenv("RWSDK33") }

project "iii_anim"
	kind "SharedLib"
	language "C++"
	targetname "iii_anim"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".dll"
	characterset ("MBCS")

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "StaticRuntime" }
		symbols "On"
		debugdir "C:/Users/aap/games/gta3"
		debugcommand "C:/Users/aap/games/gta3/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gta3\\plugins\\iii_anim.dll\""

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
		debugdir "C:/Users/aap/games/gta3"
		debugcommand "C:/Users/aap/games/gta3/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gta3\\plugins\\iii_anim.dll\""
