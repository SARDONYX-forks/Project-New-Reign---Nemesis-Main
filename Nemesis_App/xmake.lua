includes("../Nemesis_Core_Engine")

includes("@builtin/qt")
includes("../xmake/rules/gen_qrc.lua")

target("Nemesis_App", function (target)
    set_kind("binary")
    add_rules("generate_qrc")

    add_deps("Nemesis_Core_Engine_static")

    add_includedirs("include", ".")
    add_files("include/**.h")
    add_files("src/**.cpp")

    -- https://github.com/xmake-io/xmake/blob/dev/xmake/rules/qt/xmake.lua#L156
    add_rules("qt.quickapp", "qt.widgetapp")
    add_frameworks("QtMultimedia", "QtShaderTools", "QtWidgets", "QtQuick")

    if is_plat("macosx") then
        set_values("qt.bundle", true)
        add_values("qt.bundle.icon", "resources/icon.icns")
    end

    if is_plat("linux") then
        set_targetdir("bin")
        set_filename("Nemesis")
    end

    if is_plat("windows") then
        add_files("resources/app.rc")
    end
end)
