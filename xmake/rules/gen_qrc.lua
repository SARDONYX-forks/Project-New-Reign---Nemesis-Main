--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, Xmake Open Source Community.
--
-- @author      ruki
-- @file        xmake.lua
--

-- refs:
-- - https://github.com/xmake-io/xmake/blob/821545f6a78188e733b0abe84cdfcb6d07c9e082/xmake/rules/qt/qrc/xmake.lua
-- - https://github.com/xmake-io/xmake/issues/838

rule("generate_qrc", function ()
    add_deps("qt.env")

    on_config(function (target)
        import("lib.detect.find_file")

        -- get qsb
        local qt = assert(target:data("qt"), "Qt not found!")
        local search_dirs = {}
        if qt.bindir_host then table.insert(search_dirs, qt.bindir_host) end
        if qt.bindir then table.insert(search_dirs, qt.bindir) end
        if qt.libexecdir_host then table.insert(search_dirs, qt.libexecdir_host) end
        if qt.libexecdir then table.insert(search_dirs, qt.libexecdir) end
        local qsb = find_file(is_host("windows") and "qsb.exe" or "qsb", search_dirs)
        assert(os.isexec(qsb), "qsb not found!")
        -- target:data_set("qt.qsb", qsb) -- save qsb

        local qsb_exe = qsb -- target:data("qsb")

        local app_dir = target:scriptdir()

        local qml_dir = path.join(app_dir, "qml")
        local res_dir = path.join(app_dir, "resources")
        local shaders_dir = path.join(app_dir, "shaders")

        -- NOTE: qrc rules must be specified as absolute paths or the search will fail.
        local build_dir = path.absolute(path.join(target:autogendir(), "rules", "qt", "qrc"))
        local qsb_dir = path.join(build_dir, "shaders_qsb")
        local output_qrc = path.join(build_dir, "auto_resources.qrc")

        os.mkdir(build_dir)
        os.mkdir(qsb_dir)

        -- =====================================================
        -- 1. shader compile
        -- =====================================================

        local shader_exts = {".frag", ".vert"}

        for _, ext in ipairs(shader_exts) do
            local files = os.files(path.join(shaders_dir, "**" .. ext))

            for _, f in ipairs(files) do
                local out = path.join(qsb_dir, path.filename(f) .. ".qsb")
                os.execv(qsb_exe, {
                    "--qt6",
                    "--glsl", "450",
                    "--hlsl", "50",
                    "--msl", "12",
                    "-o", out,
                    f
                })
            end
        end

        -- =====================================================
        -- 2. QRC generate
        -- =====================================================

        local qrc = {}
        table.insert(qrc, "<RCC>")

        local function write_block(prefix, files, base_dir)

            if #files == 0 then return end

            table.insert(qrc, string.format("  <qresource prefix=\"%s\">", prefix))

            for _, f in ipairs(files) do
                local rel = path.relative(f, base_dir):gsub("\\", "/")
                local f_norm = f:gsub("\\", "/")

                table.insert(qrc,
                    string.format("    <file alias=\"%s\">%s</file>", rel, f_norm))
            end

            table.insert(qrc, "  </qresource>")
        end

        -- QML
        write_block("/qml",
            os.files(path.join(qml_dir, "**.qml")),
            qml_dir)

        -- PNG
        write_block("/resources",
            os.files(path.join(res_dir, "**.png")),
            res_dir)

        -- QSB
        write_block("/shaders",
            os.files(path.join(qsb_dir, "*.qsb")),
            qsb_dir)

        table.insert(qrc, "</RCC>")

        io.writefile(output_qrc, table.concat(qrc, "\n"))

        assert(os.exists(output_qrc), output_qrc .. ": Failed to generate qrc file!")
        target:add("files", output_qrc)
    end)
end)
