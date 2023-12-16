#include "animsetdata/linked/linkedstate.h"

nemesis::animsetdata::LinkedState::LinkedState(const nemesis::Line& line)
{
    throw std::runtime_error("Invalid LinkedState constructor");
}

void nemesis::animsetdata::LinkedState::GetRawName(VecNstr& lines) const
{
    Vec<LinkedCondition*> modlist;
    auto this_func = &nemesis::animsetdata::LinkedState::GetRawName;

    for (auto& condition : conditions)
    {
        switch (condition->GetType())
        {
            case nemesis::CondType::MOD_CODE:
            {
                modlist.emplace_back(condition.get());
                break;
            }
            case nemesis::CondType::FOREACH:
            {
                lines.emplace_back(nemesis::syntax::ForEach(condition->GetExpression()));
                condition->GetRawDataList(lines, this_func);
                lines.emplace_back(nemesis::syntax::Close());
                break;
            }
        }
    }

    if (modlist.empty())
    {
        if (!raw) return;

        lines.emplace_back(std::string(raw->GetName()));
        return;
    }

    if (!raw)
    {
        for (auto& cond : modlist)
        {
            lines.emplace_back(nemesis::syntax::ModCode(cond->GetExpression()));
            cond->GetRawDataList(lines, this_func);
            lines.emplace_back(nemesis::syntax::Close());
        }

        return;
    }

    Vec<VecNstr> mod_list_list;
    mod_list_list.reserve(modlist.size());

    for (auto& cond : modlist)
    {
        mod_list_list.emplace_back();
        auto& mod_lines = mod_list_list.back();
        cond->GetRawDataList(mod_lines, this_func);
        nemesis::Line mline = mod_lines.empty() ? nemesis::Line(nemesis::syntax::DeleteLine()) : mod_lines.front();
        mline += nemesis::syntax::Spaces() + nemesis::syntax::Aster(cond->GetExpression());
        lines.emplace_back(mline);
    }

    lines.emplace_back(std::string(raw->GetName()) + nemesis::syntax::Spaces()
                       + nemesis::syntax::LowerOriginal());

    for (size_t i = 0; i < mod_list_list.size(); ++i)
    {
        auto& mod_lines = mod_list_list[i];

        if (mod_lines.size() <= 1) continue;

        throw std::runtime_error("animsetdata::LinkedState does not support multi conditioned names");
    }
}

void nemesis::animsetdata::LinkedState::SetRawDataTo(VecNstr& lines) const
{
    auto templines = raw->GetLines();
    lines.insert(lines.end(), templines.begin(), templines.end());
}

void nemesis::animsetdata::LinkedState::SetNewConditionedDataTo(VecNstr& lines,
                                                                  const nemesis::Line& newline,
                                                                  const LinkedCondition& cond) const
{
    throw std::runtime_error("animsetdata::LinkedState does not support SetNewConditionedDataTo");
}

void nemesis::animsetdata::LinkedState::SetRawConditionedDataTo(VecNstr& lines) const
{
    throw std::runtime_error("animsetdata::LinkedState does not support SetRawConditionedDataTo");
}