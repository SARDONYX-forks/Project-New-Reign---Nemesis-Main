#pragma once

#include <filesystem>

#include "utilities/types.h"

namespace nemesis
{
    namespace duration
    {
        const std::string name     = "D";
        const std::string variable = "end";
        const std::string fullname = "D<end>";
    } // namespace duration

    namespace filepath
    {
        const std::string name = "FilePath";
    }

    namespace directory
    {
        const std::filesystem::path mods      = "mod\\";
        const std::filesystem::path templates = "behavior templates\\";
    } // namespace directory

    namespace filename
    {
        const std::wstring optlist = L"option_list.txt";
        const std::wstring info    = L"info.ini";
    } // namespace filename

    namespace foldername
    {
        const std::wstring animdata    = L"animationdatasinglefile";
        const std::wstring animsetdata = L"animationsetdatasinglefile";
    } // namespace foldername
} // namespace nemesis
