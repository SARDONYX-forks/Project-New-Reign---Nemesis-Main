#include "Testers/HkxBehaviorTester.h"

#include "core/CoreObject.h"

#include "hkx/hkxbehavior.h"

void nemesis::HkxBehaviorTester::Run()
{
    nemesis::TemplateClass templt_class(
        "E:\\C++\\Project New Reign - Nemesis\\test environment\\behavior_templates\\fuo\\template_info.json");
    UPtr<nemesis::AnimationRequest> request = std::make_unique<nemesis::AnimationRequest>(templt_class, true);
    UPtr<nemesis::AnimationRequest> rq      = templt_class.CreateRequest("", 1, "");

    auto model  = templt_class.GetModel("T");
    auto option = model->TryCreateOption("TDodgeStop/1.05", 1, "file.txt");
    request->AddOption(std::move(option));

    auto model2  = templt_class.GetModel("o");
    auto option2 = model2->TryCreateOption("o", 2, "file.txt");
    request->AddOption(std::move(option2));

    const std::filesystem::path filepath
        = "E:\\C++\\Project New Reign - Nemesis\\test "
          "environment\\data\\meshes\\actors\\character\\behaviors\\Nemesis_bashbehavior.xml";
    auto behavior = nemesis::HkxBehavior::ParseFromFile(filepath);

    auto request_ptr = request.get();
    nemesis::AnimationRequestRepository repo;
    nemesis::TemplateRepository templt_repo;
    repo.AddRequest(std::move(request));
    nemesis::CompilationManager compile_manager({}, repo, templt_repo);
    nemesis::CompileState& state = compile_manager.CreateCompileState(filepath);

    state.SetBaseRequest(request_ptr);
    state.QueueCurrentRequest("fuo_1", request_ptr);

    DeqNstr serialized = behavior->Serialize();
    DeqNstr compiled   = behavior->Compile(state);
}
