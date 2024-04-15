#pragma once

#include "core/NObject.h"

#include "utilities/threadpool.h"

#include "animsetdata/AnimationSetDataProject.h"

namespace nemesis
{
    struct TemplateObject;

    struct AnimationSetDataSingleFile : public nemesis::NObject
    {
    private:
        Vec<UPtr<nemesis::AnimationSetDataProject>> ProjectList;
        Vec<SPtr<nemesis::TemplateObject>> ProjectTemplateList;

        std::filesystem::path FilePath;
        std::filesystem::path TargetPath;

        std::mutex UpdaterMutex;

        static VecNstr ParseHeaders(nemesis::LineStream& stream, nemesis::SemanticManager& manager);

    public:
        void CompileTo(DeqNstr& lines, nemesis::CompileState& state) const override;
        void SerializeTo(DeqNstr& lines) const override;

        UPtr<nemesis::NObject> CloneNObject() const override;
        UPtr<nemesis::AnimationSetDataSingleFile> Clone() const;

        void CompileFile(nemesis::CompileState& state) const;
        void CompileFileAs(const std::filesystem::path& filepath, nemesis::CompileState& state) const;

        std::filesystem::path GetFilePath() const;
        std::filesystem::path GetTargetPath() const;

        UPtr<nemesis::AnimationSetDataProject>& AddProject(UPtr<nemesis::AnimationSetDataProject>&& project);
        SPtr<nemesis::TemplateObject>& AddProjectTemplate(const SPtr<nemesis::TemplateObject>& templt_obj);

        nemesis::AnimationSetDataProject* GetProject(const std::string& project_name);
        const nemesis::AnimationSetDataProject* GetProject(const std::string& project_name) const;

        void SerializeToDirectory(const std::filesystem::path& directory_path) const;
        static UPtr<nemesis::AnimationSetDataSingleFile>
        DeserializeFromDirectory(const std::filesystem::path& directory_path);

        static UPtr<nemesis::AnimationSetDataSingleFile> ParseFromFile(const std::filesystem::path& filepath);
        static UPtr<nemesis::AnimationSetDataSingleFile> ParseFromFile(const std::filesystem::path& filepath,
                                                                       nemesis::ThreadPool& threadpool);
    };
}
