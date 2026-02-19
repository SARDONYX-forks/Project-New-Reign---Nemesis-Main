-- NOTE: Prevent `Nemesis_Havok` from exhibiting unintended behavior during optimization
rule("link_archive_all", function()
    on_config(function(target)
        local lib_file = path.absolute(target:targetfile()) -- .lib file path

        if target:is_plat("windows") then
            target:add("cxflags", "/bigobj")
            target:add("ldflags", "/WHOLEARCHIVE:" .. lib_file, { force = true })
        elseif target:is_plat("macosx") then
            target:add("ldflags", "-Wl,-force_load," .. lib_file, { force = true })
        elseif target:is_plat("linux") then
            target:add("ldflags", "-Wl,--whole-archive," .. lib_file .. ",--no-whole-archive", { force = true })
        end
    end)
end)
