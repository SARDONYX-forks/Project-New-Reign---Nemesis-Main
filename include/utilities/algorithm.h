#pragma once

#include <string>

namespace nemesis
{
    const char* to_lower_copy(const char* data);
    const wchar_t* to_lower_copy(const wchar_t* data);

    std::string to_lower_copy(const std::string& data);
    std::wstring to_lower_copy(const std::wstring& data);

    void to_lower(std::string& data);
    void to_lower(std::wstring& data);

    const char* to_upper_copy(const char* data);
    const wchar_t* to_upper_copy(const wchar_t* data);

    std::string to_upper_copy(const std::string& data);
    std::wstring to_upper_copy(const std::wstring& data);

    void to_upper(std::string& data);
    void to_upper(std::wstring& data);

    bool iequals(const char* l, const char* r);
    bool iequals(const wchar_t* l, const wchar_t* r);
    bool iequals(const char* l, const std::string& r);
    bool iequals(const wchar_t* l, const std::wstring& r);
    bool iequals(const std::string& l, const char* r);
    bool iequals(const std::wstring& l, const wchar_t* r);
    bool iequals(const std::string& l, const std::string& r);
    bool iequals(const std::wstring& l, const std::wstring& r);

    template <typename T, typename F>
    inline T transform_to(const F& str) noexcept
    {
        if (str.empty()) return {};

        return {std::begin(str), std::end(str)};
    };

    // Specialization for converting std::string to std::wstring
    template <>
    inline std::wstring transform_to<std::wstring, std::string>(const std::string& str) noexcept
    {
        std::wstring wstr;
        size_t size;
        wstr.resize(str.length());
        mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
        return wstr;
    }

    template <>
    inline std::string transform_to<std::string, std::wstring>(const std::wstring& wstr) noexcept
    {
        std::string str;
        size_t size;
        str.resize(wstr.length());
        wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
        return str;
    }

    // Specialization for converting std::string to std::wstring
    template <>
    inline std::wstring transform_to<std::wstring, std::string_view>(const std::string_view& str) noexcept
    {
        std::wstring wstr;
        size_t size;
        wstr.resize(str.length());
        mbstowcs_s(&size, &wstr[0], wstr.size() + 1, std::string(str).data(), str.size());
        return wstr;
    }

    template <>
    inline std::string transform_to<std::string, std::wstring_view>(const std::wstring_view& wstr) noexcept
    {
        std::string str;
        size_t size;
        str.resize(wstr.length());
        wcstombs_s(&size, &str[0], str.size() + 1, std::wstring(wstr).data(), wstr.size());
        return str;
    }

} // namespace nemesis
