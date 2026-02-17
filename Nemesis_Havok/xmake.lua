target("Nemesis_Havok_static", function()
    set_kind("static")

    add_includedirs("..", { public = true })
    add_includedirs("include", { public = true })
    add_headerfiles("include/**.h", "include/**.hpp", { public = true })

    add_files("src/**.cpp|main.cpp")
end)

target("Nemesis_Havok", function()
    set_kind("binary")

    add_deps("Nemesis_Havok_static")
    add_files("src/main.cpp")

    on_config(link_archive_all)
end)
