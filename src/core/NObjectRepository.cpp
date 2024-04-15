#include "core/NObjectRepository.h"
#include "core/CompilationManager.h"
#include "core/ModRepository.h"

#include "hkx/HkxBehavior.h"
#include "hkx/HkxCharacter.h"

#include "animdata/AnimationDataSingleFile.h"

#include "animsetdata/AnimationSetDataSingleFile.h"

#include "utilities/threadpool.h"
#include "utilities/stringextension.h"

#include "nemesisinfo.h"

namespace sf = std::filesystem;

void nemesis::NObjectRepository::ParseHkxFilesFromDirectory(const std::filesystem::path& dir,
                                                            nemesis::ThreadPool& threadpool)
{
    for (auto& entry : sf::directory_iterator(dir))
    {
        const sf::path& path = entry.path();

        if (entry.is_directory())
        {
            ParseHkxFilesFromDirectory(path, threadpool);
            continue;
        }

        if (path.extension().wstring() != L".xml") continue;

        if (!nemesis::istarts_with(path.stem().wstring(), L"nemesis_")) continue;

        if (nemesis::istarts_with(path.parent_path().stem().wstring(), L"characters"))
        {
            auto character = nemesis::HkxCharacter::ParseFromFile(path, threadpool);
            Characters.emplace_back(std::move(character));
            continue;
        }

        auto behavior = nemesis::HkxBehavior::ParseFromFile(path, threadpool);
        Behaviors.emplace_back(std::move(behavior));
    }
}

void nemesis::NObjectRepository::PatchNodeList(
    const Vec<const nemesis::ModPatch<nemesis::HkxNode>*> node_list)
{
    auto nemesis_info = NemesisInfo::GetInstance();

    for (auto& node : node_list)
    {
        std::filesystem::path filepath = node->GetFilePath();
        std::filesystem::path relative_path
            = filepath.parent_path().wstring().substr(
                  (nemesis_info->GetDataPath() / "nemesis_engine" / "mods").wstring().length() + 1)
              + L".xml";

        auto behavior = GetBehavior(relative_path);
        auto& m_node  = node->GetContent();

        if (behavior)
        {
            auto o_node = behavior->GetNodeById(filepath.stem().string());

            if (o_node)
            {
                o_node->MatchAndUpdate(m_node);
                continue;
            }

            behavior->AddModNode(node->GetModClass().GetCode(), m_node.Clone());
            continue;
        }

        auto character = GetCharacter(relative_path);

        if (!character) continue;

        auto o_node = character->GetNodeById(filepath.stem().string());

        if (o_node)
        {
            o_node->MatchAndUpdate(m_node);
            continue;
        }

        character->AddModNode(node->GetModClass().GetCode(), m_node.Clone());
    }
}

void nemesis::NObjectRepository::PatchHeaderList(
    const Vec<const nemesis::ModPatch<nemesis::AnimationDataProject::Headers>*> header_list)
{
    for (auto& m_header : header_list)
    {
        auto filepath = m_header->GetFilePath();
        auto folder   = filepath.parent_path().stem().string();

        static nemesis::regex rgx("^([^~]+)~([0-9]+)");
        nemesis::smatch proj_match;

        if (!nemesis::regex_match(folder, proj_match, rgx)) continue;

        auto proj_name = proj_match[1];
        size_t index   = std::stoul(proj_match[2]);
        auto project   = AnimDataSingleFile->GetProject(proj_name, index);

        if (!project)
        {
            auto uproject = std::make_unique<nemesis::AnimationDataProject>(proj_name);
            project       = uproject.get();
            AnimDataSingleFile->AddProject(std::move(uproject));
        }

        project->MatchAndUpdateHeader(m_header->GetContent());
    }
}

void nemesis::NObjectRepository::PatchClipData(
    const Vec<const nemesis::ModPatch<nemesis::AnimationDataClipData>*> clip_data_list)
{
    for (auto& m_clip_data : clip_data_list)
    {
        auto filepath = m_clip_data->GetFilePath();
        auto folder   = filepath.parent_path().stem().string();

        static nemesis::regex rgx("^([^~]+)~([0-9]+)");
        nemesis::smatch proj_match;

        if (!nemesis::regex_match(folder, proj_match, rgx)) continue;

        auto proj_name = proj_match[1];
        size_t index   = std::stoul(proj_match[2]);
        auto project   = AnimDataSingleFile->GetProject(proj_name, index);

        if (!project)
        {
            auto uproject = std::make_unique<nemesis::AnimationDataProject>(proj_name);
            project       = uproject.get();
            AnimDataSingleFile->AddProject(std::move(uproject));
        }

        auto& m_data   = m_clip_data->GetContent();
        auto clip_data = project->GetClipData(m_data.GetName(), m_data.GetCode());

        if (clip_data)
        {
            clip_data->MatchAndUpdate(m_data);
            continue;
        }

        auto u_clip_data = m_data.Clone(m_clip_data->GetModClass(), m_clip_data->GetFilePath());
        project->AddClipData(std::move(u_clip_data));
    }
}

void nemesis::NObjectRepository::PatchMotionData(
    const Vec<const nemesis::ModPatch<nemesis::AnimationDataMotionData>*> motion_data_list)
{
    for (auto& m_motion_data : motion_data_list)
    {
        auto filepath = m_motion_data->GetFilePath();
        auto folder   = filepath.parent_path().stem().string();

        static nemesis::regex rgx("^([^~]+)~([0-9]+)");
        nemesis::smatch proj_match;

        if (!nemesis::regex_match(folder, proj_match, rgx)) continue;

        auto proj_name = proj_match[1];
        size_t index   = std::stoul(proj_match[2]);
        auto project   = AnimDataSingleFile->GetProject(proj_name, index);

        if (!project)
        {
            auto uproject = std::make_unique<nemesis::AnimationDataProject>(proj_name);
            project       = uproject.get();
            AnimDataSingleFile->AddProject(std::move(uproject));
        }

        auto& m_data     = m_motion_data->GetContent();
        auto motion_data = project->GetMotionData(m_data.GetCode());

        if (motion_data)
        {
            motion_data->MatchAndUpdate(m_data);
            continue;
        }

        auto u_motion_data = m_data.Clone(m_motion_data->GetModClass(), m_motion_data->GetFilePath());
        project->AddMotionData(std::move(u_motion_data));
    }
}

void nemesis::NObjectRepository::PatchStateData(
    const Vec<const nemesis::ModPatch<nemesis::AnimationSetDataState>*> statedata_list)
{
    for (auto& m_statedata : statedata_list)
    {
        auto filepath  = m_statedata->GetFilePath();
        auto proj_name = filepath.parent_path().filename().string();
        nemesis::replace(proj_name, "~", "\\");
        auto project = AnimSetDataSingleFile->GetProject(proj_name);

        if (!project)
        {
            auto uproject = std::make_unique<nemesis::AnimationSetDataProject>(proj_name);
            project       = uproject.get();
            AnimSetDataSingleFile->AddProject(std::move(uproject));
        }

        auto& m_data    = m_statedata->GetContent();
        auto state_data = project->GetState(m_data.GetName());

        if (state_data)
        {
            state_data->MatchAndUpdate(m_data);
            continue;
        }

        auto ustate_data = m_data.Clone(m_statedata->GetModClass(), m_statedata->GetFilePath());
        project->AddState(std::move(ustate_data));
    }
}

nemesis::NObjectRepository::NObjectRepository(const std::filesystem::path& data_path)
{
    nemesis::ThreadPool threadpool;

    sf::path meshes_path = data_path / L"meshes";
    ParseHkxFilesFromDirectory(meshes_path, threadpool);

    AnimDataSingleFile = nemesis::AnimationDataSingleFile::ParseFromFile(
        meshes_path / L"nemesis_animationdatasinglefile.txt", threadpool);
    AnimSetDataSingleFile = nemesis::AnimationSetDataSingleFile::ParseFromFile(
        meshes_path / L"nemesis_animationsetdatasinglefile.txt", threadpool);

    threadpool.join_all();
}

nemesis::HkxBehavior*
nemesis::NObjectRepository::GetBehavior(const std::filesystem::path& relative_path) noexcept
{
    for (auto& behavior : Behaviors)
    {
        if (!nemesis::iequals(behavior->GetRelativePath().wstring(), relative_path.wstring())) continue;

        return behavior.get();
    }

    return nullptr;
}

const nemesis::HkxBehavior*
nemesis::NObjectRepository::GetBehavior(const std::filesystem::path& relative_path) const noexcept
{
    for (auto& behavior : Behaviors)
    {
        if (!nemesis::iequals(behavior->GetRelativePath().wstring(), relative_path.wstring())) continue;

        return behavior.get();
    }

    return nullptr;
}

nemesis::HkxCharacter*
nemesis::NObjectRepository::GetCharacter(const std::filesystem::path& relative_path) noexcept
{
    for (auto& character : Characters)
    {
        if (!nemesis::iequals(character->GetRelativePath().wstring(), relative_path.wstring())) continue;

        return character.get();
    }

    return nullptr;
}

const nemesis::HkxCharacter*
nemesis::NObjectRepository::GetCharacter(const std::filesystem::path& relative_path) const noexcept
{
    for (auto& character : Characters)
    {
        if (!nemesis::iequals(character->GetRelativePath().wstring(), relative_path.wstring())) continue;

        return character.get();
    }

    return nullptr;
}

nemesis::AnimationDataSingleFile* nemesis::NObjectRepository::GetAnimDataSingleFile() noexcept
{
    return AnimDataSingleFile.get();
}

const nemesis::AnimationDataSingleFile* nemesis::NObjectRepository::GetAnimDataSingleFile() const noexcept
{
    return AnimDataSingleFile.get();
}

nemesis::AnimationSetDataSingleFile* nemesis::NObjectRepository::GetAnimSetDataSingleFile() noexcept
{
    return AnimSetDataSingleFile.get();
}

const nemesis::AnimationSetDataSingleFile*
nemesis::NObjectRepository::GetAnimSetDataSingleFile() const noexcept
{
    return AnimSetDataSingleFile.get();
}

void nemesis::NObjectRepository::Patch(const nemesis::ModRepository& mod_repo)
{
    auto mod_class_list = mod_repo.GetModClassList();

    for (auto& mod_class : mod_class_list)
    {
        PatchNodeList(mod_class->GetNodeList());
        PatchStateData(mod_class->GetStateList());
        PatchHeaderList(mod_class->GetAnimDataHeaderList());
        PatchClipData(mod_class->GetClipDataList());
        PatchMotionData(mod_class->GetMotionDataList());
    }
}

void nemesis::NObjectRepository::Compile(nemesis::CompilationManager& manager)
{
    nemesis::ThreadPool cthread_pool;
    nemesis::ThreadPool thread_pool;

    for (auto& character : Characters)
    {
        auto& state = manager.CreateCompileState(character->GetFilePath());
        cthread_pool.enqueue([&character, &state]() { character->CompileFile(state); });
    }

    cthread_pool.join_all();

    for (auto& behavior : Behaviors)
    {
        auto& state = manager.CreateCompileState(behavior->GetFilePath());
        thread_pool.enqueue([&behavior, &state]() { behavior->CompileFile(state); });
    }

    auto& adsf_state = manager.CreateCompileState(AnimDataSingleFile->GetFilePath());
    thread_pool.enqueue([this, &adsf_state]() { AnimDataSingleFile->CompileFile(adsf_state); });

    auto& asdsf_state = manager.CreateCompileState(AnimSetDataSingleFile->GetFilePath());
    thread_pool.enqueue([this, &asdsf_state]() { AnimSetDataSingleFile->CompileFile(asdsf_state); });
    thread_pool.join_all();
}
