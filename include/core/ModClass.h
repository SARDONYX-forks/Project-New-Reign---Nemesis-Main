#pragma once

#include "core/modinfo.h"
#include "core/ModPatch.h"

#include "hkx/HkxNode.h"

#include "animdata/AnimationDataProject.h"
#include "animdata/AnimationDataClipData.h"
#include "animdata/AnimationDataMotionData.h"

#include "animsetdata/AnimationSetDataState.h"

namespace nemesis
{
    struct ModClass
    {
    private:
        nemesis::ModInfo Info;
        nemesis::regex ModRgx;

        Vec<UPtr<nemesis::ModPatch<nemesis::HkxNode>>> NodeList;
        Vec<UPtr<nemesis::ModPatch<nemesis::AnimationSetDataState>>> StateList;
        Vec<UPtr<nemesis::ModPatch<nemesis::AnimationDataProject::Headers>>> AnimDataHeaderList;
        Vec<UPtr<nemesis::ModPatch<nemesis::AnimationDataClipData>>> ClipDataList;
        Vec<UPtr<nemesis::ModPatch<nemesis::AnimationDataMotionData>>> MotionDataList;

        mutable UPtr<Vec<const nemesis::ModPatch<nemesis::HkxNode>*>> NodeListCache;
        mutable UPtr<Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*>> StateListCache;
        mutable UPtr<Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*>>
            AnimDataHeaderListCache;
        mutable UPtr<Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*>> ClipDataListCache;
        mutable UPtr<Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*>> MotionDataListCache;

        void ParseModFromMeshesDirectory(const std::filesystem::path& meshes_dir_path);
        void ParseModHkxFromDirectory(const std::filesystem::path& dir_path);
        void ParseModAnimDataFromDirectory(const std::filesystem::path& dir_path);
        void ParseModAnimSetDataFromDirectory(const std::filesystem::path& dir_path);

    public:
        ModClass(const std::filesystem::path& ini_file);

        std::string GetCode() const noexcept;
        Vec<const nemesis::ModPatch<nemesis::HkxNode>*> GetNodeList() const;
        Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*> GetStateList() const;
        Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*> GetAnimDataHeaderList() const;
        Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*> GetClipDataList() const;
        Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*> GetMotionDataList() const;
    };
}
