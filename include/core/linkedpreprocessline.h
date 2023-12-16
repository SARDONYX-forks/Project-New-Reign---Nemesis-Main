#pragma once

#include "core/linked.h"
#include "core/linkedcond.h"
#include "core/preprocessline.h"
#include "utilities/conditiondetails.h"

namespace nemesis
{
    struct HkxBehaviorFile;
    struct LineProcess;
    struct PreprocessLine;
    struct Template;

    struct LinkedPreprocessLine
        : public nemesis::Linked<nemesis::PreprocessLine, nemesis::LinkedPreprocessLine>
    {
        using LinkedCondition = nemesis::LinkedCond<nemesis::PreprocessLine, nemesis::LinkedPreprocessLine>;

        LinkedPreprocessLine() = default;
        LinkedPreprocessLine(const nemesis::Line& line) noexcept;
        LinkedPreprocessLine(
            const nemesis::Linked<nemesis::PreprocessLine, nemesis::LinkedPreprocessLine>& line) noexcept;

        void TryAddLineProcess(SPtr<nemesis::LineProcess>& processptr);
        void AddBehavior(nemesis::HkxBehaviorFile& behavior);
        void AddTemplate(nemesis::Template& templateref);

        void SetToStandAlone();
        static void SetToStandAlone(nemesis::LinkedPreprocessLine& linkedline);
        static void SetToStandAlone(LinkedCondition& linkedcondition);

        bool HasProcess() const;
        std::string RawToString() const override;

        VecNstr GetProcessedLines(nemesis::ScopeInfo& scopeinfo) const;
    };
} // namespace nemesis
