-- pip install aqt-install
-- aqt install-qt windows desktop 6.10.2 win64_msvc2022_64 -m qt5compat qtmultimedia qtshadertools -O packages/
-- ./.github/gen_qrc.ps1
-- xmake f --qt=./packages/qt/6.10.2/msvc2022_64 --toolchain=clang-cl -y
set_project("Nemesis_Engine")
set_version("0.8.7")
set_license("GPL-3.0")

set_languages("c++23")

add_rules("mode.debug", "mode.releasedbg")
set_defaultmode("debug")

-- NOTE: Prevent `Nemesis_Havok` from exhibiting unintended behavior during optimization
function link_archive_all(target)
        local dep = target:dep("Nemesis_Havok_static")
        local depfile = dep:targetfile()

        if target:is_plat("windows") then
            target:add("cxflags", "/bigobj")
            target:add("ldflags",
                "/WHOLEARCHIVE:" .. depfile,
                { force = true }
            )

        elseif target:is_plat("macosx") then
            target:add("ldflags",
                "-Wl,-force_load," .. depfile,
                { force = true }
            )

        elseif target:is_plat("linux") then
            target:add("ldflags",
                "-Wl,--whole-archive",
                depfile,
                "-Wl,--no-whole-archive",
                { force = true }
            )
        end
    end

includes("./Nemesis_App")
