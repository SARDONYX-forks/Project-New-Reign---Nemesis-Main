#pragma once
#pragma once

#include "core/LineStream.h"
#include "core/SubTemplateObject.h"

#include "core/Template/TemplateObject.h"

#include "utilities/threadpool.h"

#include "hkx/HkxNode.h"

namespace nemesis
{
    struct SemanticManager;

    struct HkxFile : public nemesis::NObject
    {
    protected:
        Map<std::string, nemesis::HkxNode*> NodeMap;

        nemesis::HkxNode* RootNode;

        UPtr<nemesis::CollectionObject> NewNodes;
        UPtr<nemesis::CollectionObject> RegularNodes;

        UMap<std::string, Map<size_t, SPtr<nemesis::TemplateObject>>> TemplateMap;

        USetStr ModInUsedList;

        std::filesystem::path FilePath;
        std::filesystem::path TargetPath;
        std::filesystem::path CachedFilePath;
        std::filesystem::path RelativePath;

        void CompileTemplate(const nemesis::AnimationRequest* request,
                             DeqNstr& lines,
                             nemesis::CompileState& state) const;
        DeqNstr CompileAllTemplates(nemesis::CompileState& state) const;
        DeqNstr CompileAllSubTemplates(nemesis::CompileState& state) const;

    public:
        void CompileFile(nemesis::CompileState& state) const;
        void CompileFileAs(const std::filesystem::path& filepath, nemesis::CompileState& state) const;

        void AddTemplate(const SPtr<nemesis::TemplateObject>& templt_obj);

        nemesis::HkxNode* AddModNode(const std::string& modcode, UPtr<nemesis::HkxNode>&& node);

        nemesis::HkxNode* GetNodeById(const std::string& node_id);
        const nemesis::HkxNode* GetNodeById(const std::string& node_id) const;

        const std::filesystem::path& GetFilePath() const noexcept;
        const std::filesystem::path& GetTargetPath() const noexcept;
        const std::filesystem::path& GetCachedFilePath() const noexcept;
        const std::filesystem::path& GetRelativePath() const noexcept;

        bool IsSameAsCached(nemesis::CompileState& state) const;

    protected:
        static bool TryGetValueInHkcString(const std::string& line, std::string& value);
    };
}
