set_xmakever("2.8.5")

set_kind("binary")
set_languages("c++17")
set_version("0.84")
set_warnings("allextra")

-- allowed
set_allowedarchs("x64") -- No need build hkxcmd, so we can use x64
set_allowedmodes("debug", "releasedbg")

-- default
set_defaultarchs("x64")
set_defaultmode("releasedbg")

-- rules
add_rules("mode.debug", "mode.releasedbg")

-- policies
set_policy("package.requires_lock", true)

-- packages
-- ref: https://xmake.io/mirror/manual/global_interfaces.html#:~:text=Install%20third%2Dparty%20manager%20package
add_requires("conan::boost/1.72.0", { alias = "boost" }) -- https://conan.io/center/recipes/boost

includes("src")
