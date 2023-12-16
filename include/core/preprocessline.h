#pragma once

#include "utilities/line.h"
#include "utilities/types.h"

namespace nemesis
{
    struct File;
    struct ScopeInfo;
    struct Template;
    struct HkxBehaviorFile;
    struct LineProcess;

    struct PreprocessLine : public nemesis::Line
    {
    private:
        SPtr<nemesis::LineProcess> processptr     = nullptr;
        const nemesis::File* file                 = nullptr;
        const nemesis::Template* templateptr      = nullptr;
        const nemesis::HkxBehaviorFile* pbehavior = nullptr;
        VecStr cacheblocks;

    public:
        PreprocessLine(const nemesis::Line& line) noexcept;
        PreprocessLine(const nemesis::PreprocessLine& linkedline) noexcept;

        void AddBehavior(nemesis::HkxBehaviorFile& behavior);
        void AddTemplate(nemesis::Template& templateref);
        void TryAddLineProcess(SPtr<nemesis::LineProcess>& processptr);

        bool IsStandAlone();
        void SetToStandAlone();

        const nemesis::File* GetFile() const noexcept;
        const nemesis::Template* GetTemplate() const noexcept;
        const nemesis::LineProcess* GetProcess() const noexcept;
        VecStr GetByBlocks() const noexcept;

        bool HasProcess() const noexcept;
        nemesis::Line Process(nemesis::ScopeInfo& scopeinfo) const;
        //VecNstr CondiitonOutput(nemesis::ScopeInfo& scopeinf) const;
    };
} // namespace nemesis