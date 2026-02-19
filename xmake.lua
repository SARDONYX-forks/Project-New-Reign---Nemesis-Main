-- pip install aqt-install
-- aqt install-qt windows desktop 6.10.2 win64_msvc2022_64 -m qt5compat qtmultimedia qtshadertools -O packages/
-- xmake f --qt=./packages/qt/6.10.2/msvc2022_64 --toolchain=clang-cl -y
set_project("Nemesis_Engine")
set_version("0.8.7")
set_license("GPL-3.0")

set_languages("c++23")

add_rules("mode.debug", "mode.releasedbg")

-- NOTE: It is extremely important to keep optimization at -O2. Otherwise, the compiler will destroy the code logic.
--
-- - https://xmake.io/posts/quickstart-8-switch-build-mode.html#using-mode-variables
-- - https://xmake.io/api/description/project-target.html#set-optimize
if is_mode("release") then
    set_optimize("fastest")
end

includes("./Nemesis_App")
