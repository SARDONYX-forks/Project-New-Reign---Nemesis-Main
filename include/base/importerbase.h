#pragma once

#include "core/animvarptr.h"

#include "utilities/types.h"

namespace nemesis
{
    class regex;
    struct File;
    struct LexerSearch;
    struct ProcessParser;
    struct RegexSearch;

    struct ImporterBase
    {
    protected:
        UPtr<nemesis::regex> stateid_rgx = nullptr;
        UPtr<nemesis::regex> animobj_rgx = nullptr;
        Vec<nemesis::RegexSearch> animidrgx_list;
        Vec<nemesis::RegexSearch> groupidrgx_list;
        Vec<nemesis::RegexSearch> masteridrgx_list;

        Vec<UPtr<nemesis::LexerSearch>> lexersearch_list;

        Vec<Pair<std::string_view, nemesis::AnimVarPtr::Lexer>> varlexer_list;

        bool bCountElement = false;
        bool bEnding       = false;
        int iElementRange  = false;

        VecNstr filelines;
        nemesis::File& file;

    public:
        ImporterBase(nemesis::File& file)
            : file(file)
        {
        }

        virtual ~ImporterBase() {}

        nemesis::File& GetFile()
        {
            return file;
        }

        friend nemesis::ProcessParser;
    };
} // namespace nemesis
