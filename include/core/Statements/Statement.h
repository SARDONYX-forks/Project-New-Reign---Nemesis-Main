#pragma once

#include <filesystem>
#include <functional>

#include "utilities/types.h"

namespace nemesis
{
    struct Line;
    struct CompileState;
    struct TemplateClass;
    struct SemanticManager;
    struct AnimationRequest;

	struct Statement
    {
    protected:
        std::string Expression;
        size_t LineNum = 0;
        std::filesystem::path FilePath;
        VecStr Components;

        Statement(const std::string& expression, size_t linenum, const std::filesystem::path filepath, bool no_component = false);
        Statement(const nemesis::Line& expression, bool no_component = false);
        Statement(const nemesis::Statement& statement);

        virtual SPtr<std::function<bool(nemesis::CompileState&)>> CallbackTargetRequests(
            const nemesis::TemplateClass& templt_class,
            const nemesis::SemanticManager& manager,
            const std::function<bool(nemesis::CompileState&, const nemesis::AnimationRequest*)>& callback);

        virtual SPtr<std::function<const nemesis::AnimationRequest*(nemesis::CompileState&)>>
        GetTargetRequest(const nemesis::TemplateClass& templt_class, const nemesis::SemanticManager& manager);

        const nemesis::AnimationRequest* GetBaseRequest(nemesis::CompileState& state) const;

    public:
        const std::string& GetExpression() const noexcept;
        size_t GetLineNum() const noexcept;
        const std::filesystem::path& GetFilePath() const noexcept;
        size_t GetTemplateNumber(const nemesis::TemplateClass& templt_class) const;

        virtual std::string Serialize() const = 0;

        static VecStr SplitComponents(const std::string& value);
    };
}
