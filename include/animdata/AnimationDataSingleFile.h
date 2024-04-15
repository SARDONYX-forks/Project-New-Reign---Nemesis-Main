#pragma once

#include "core/NObject.h"

#include "utilities/threadpool.h"

#include "animdata/AnimationDataProject.h"

namespace nemesis
{
    struct TemplateObject;

    struct AnimationDataSingleFile : public nemesis::NObject
    {
    private:
        Vec<UPtr<nemesis::AnimationDataProject>> ProjectList;
        Vec<SPtr<nemesis::TemplateObject>> ProjectTemplateList;

        std::filesystem::path FilePath;
        std::filesystem::path TargetPath;
        std::filesystem::path RelativePath;

        std::mutex UpdaterMutex;

        static VecNstr ParseHeaders(nemesis::LineStream& stream, nemesis::SemanticManager& manager);

    public:
        void CompileTo(DeqNstr& lines, nemesis::CompileState& state) const override;
        void SerializeTo(DeqNstr& lines) const override;

        UPtr<nemesis::NObject> CloneNObject() const override;
        UPtr<nemesis::AnimationDataSingleFile> Clone() const;

        void CompileFile(nemesis::CompileState& state) const;
        void CompileFileAs(const std::filesystem::path& filepath, nemesis::CompileState& state) const;

        std::filesystem::path GetFilePath() const;
        std::filesystem::path GetTargetPath() const;
        std::filesystem::path GetRelativePath() const;

        nemesis::AnimationDataProject* GetProject(const std::string& project_name, size_t order_position);
        UPtr<nemesis::AnimationDataProject>& AddProject(UPtr<nemesis::AnimationDataProject>&& project);
        SPtr<nemesis::TemplateObject>& AddProjectTemplate(const SPtr<nemesis::TemplateObject>& templt_obj);

        void SerializeToDirectory(const std::filesystem::path& directory_path) const;
        static UPtr<nemesis::AnimationDataSingleFile>
        DeserializeFromDirectory(const std::filesystem::path& directory_path);

        static UPtr<nemesis::AnimationDataSingleFile> ParseFromFile(const std::filesystem::path& filepath);
        static UPtr<nemesis::AnimationDataSingleFile> ParseFromFile(const std::filesystem::path& filepath,
                                                                    nemesis::ThreadPool& threadpool);
    };
}
