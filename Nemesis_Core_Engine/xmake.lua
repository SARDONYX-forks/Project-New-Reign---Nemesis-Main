add_requires("exprtk =0.0.3", { configs = { header_only = true } })
add_requires("nlohmann_json 3.11.3")
add_requires("python 3.13.2") -- NOTE: The debug library cannot be found, causing an error. Please build in release mode.

includes("../Nemesis_Havok")

target("Nemesis_Core_Engine_static", function ()
    set_kind("static")
    add_deps("Nemesis_Havok_static")

    add_packages("exprtk", "nlohmann_json", "python")

    add_includedirs("../Nemesis_Core_Engine/include", { public = true })
    add_includedirs("include")
    add_headerfiles("include/**.h")
    add_files("src/**.cpp|main.cpp")
end)

target("Nemesis_Core_Engine", function ()
    set_kind("binary")

    add_deps("Nemesis_Core_Engine_static")
    add_packages("exprtk", "nlohmann_json", "python")

    add_files("src/main.cpp")
end)
