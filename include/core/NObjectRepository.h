#pragma once

#include "animdata/AnimationDataProject.h"

namespace nemesis
{
    template<typename Ty>
    struct ModPatch;

    struct HkxNode;
    struct ThreadPool;
    struct HkxBehavior;
    struct HkxCharacter;
    struct ModRepository;
    struct CollectionObject;
    struct CompilationManager;
    struct AnimationSetDataState;
    struct AnimationDataClipData;
    struct AnimationDataMotionData;
    struct AnimationDataSingleFile;
    struct AnimationSetDataSingleFile;
    struct AnimationRequestRepository;

	struct NObjectRepository
    {
    private:
        Vec<UPtr<nemesis::HkxCharacter>> Characters;
        Vec<UPtr<nemesis::HkxBehavior>> Behaviors;
        UPtr<nemesis::AnimationDataSingleFile> AnimDataSingleFile;
        UPtr<nemesis::AnimationSetDataSingleFile> AnimSetDataSingleFile;

        void ParseHkxFilesFromDirectory(const std::filesystem::path& dir, nemesis::ThreadPool& threadpool);

        void PatchNodeList(const Vec<const nemesis::ModPatch<nemesis::HkxNode>*> node_list);
        void PatchHeaderList(
            const Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*> header_list);
        void
        PatchClipData(const Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*> clip_data_list);
        void
        PatchMotionData(const Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*> motion_data_list);
        void
        PatchStateData(const Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*> state_list);

    public:
        NObjectRepository(const std::filesystem::path& data_path);

        nemesis::HkxBehavior* GetBehavior(const std::filesystem::path& relative_path) noexcept;
        const nemesis::HkxBehavior* GetBehavior(const std::filesystem::path& relative_path) const noexcept;
        nemesis::HkxCharacter* GetCharacter(const std::filesystem::path& relative_path) noexcept;
        const nemesis::HkxCharacter* GetCharacter(const std::filesystem::path& relative_path) const noexcept;
        nemesis::AnimationDataSingleFile* GetAnimDataSingleFile() noexcept;
        const nemesis::AnimationDataSingleFile* GetAnimDataSingleFile() const noexcept;
        nemesis::AnimationSetDataSingleFile* GetAnimSetDataSingleFile() noexcept;
        const nemesis::AnimationSetDataSingleFile* GetAnimSetDataSingleFile() const noexcept;

        void Patch(const nemesis::ModRepository& mod_repo);

        void Compile(nemesis::CompilationManager& manager);
    };
}
