#include "core/AnimationRequestRepository.h"
#include "core/Template.h"

void nemesis::AnimationRequestRepository::AddRequestsFromFile(const std::filesystem::path& filepath,
                                                              nemesis::TemplateRepository& templt_repo)
{
    VecNstr lines;
    GetFileLines(filepath, lines, false);
    Vec<nemesis::AnimationRequest*> request_layers;

    for (auto& line : lines)
    {
        auto request = templt_repo.CreateRequest(line, line.GetLineNumber(), filepath);

        if (request)
        {
            request_layers = {request.get()};
            AddRequest(std::move(request));
            continue;
        }

        if (nemesis::starts_with(line, "MD "))
        {
            request_layers.back()->SetMotionData(line.substr(3));
            continue;
        }

        if (nemesis::starts_with(line, "RD "))
        {
            request_layers.back()->SetRotationData(line.substr(3));
            continue;
        }

        static nemesis::regex ext_rgx("^([\\+]+) .+$");
        nemesis::smatch match;

        if (!nemesis::regex_match(line, match, ext_rgx)) continue;

        std::string plus  = match[1];
        size_t length     = plus.length();
        size_t layer_size = request_layers.size();

        if (length == layer_size)
        {
            auto parent = request_layers.back();
            auto child_request
                = parent->GetTemplateClass().CreateRequest(line, line.GetLineNumber(), filepath);
            request_layers.emplace_back(child_request.get());
            parent->AddRequest(std::move(child_request));
            continue;
        }

        if (length > layer_size)
        {
            throw std::runtime_error("Invalid Command: Parent level (" + std::to_string(length)
                                     + ") cannot be found (Line: " + std::to_string(line.GetLineNumber())
                                     + ", File: " + filepath.string() + ")");
        }

        while (length + 1 < request_layers.size())
        {
            request_layers.pop_back();
        }

        auto parent = request_layers[length - 1];
        auto child_request
            = parent->GetTemplateClass().CreateRequest(line, line.GetLineNumber(), filepath);
        request_layers.back() = child_request.get();
        parent->AddRequest(std::move(child_request));
    }
}

void nemesis::AnimationRequestRepository::AddRequestsFromDirectory(const std::filesystem::path& dir,
                                                                   nemesis::TemplateRepository& templt_repo)
{
    for (auto& entry : std::filesystem::directory_iterator(dir))
    {
        auto path = entry.path();

        if (entry.is_directory())
        {
            AddRequestsFromDirectory(path, templt_repo);
            continue;
        }

        if (path.extension().wstring() != L".txt") continue;

        std::wstring lower_filename = path.stem().wstring();

        if (!nemesis::starts_with(lower_filename, L"fnis_") && !nemesis::starts_with(lower_filename, L"nemesis_"))
        {
            continue;
        }

        AddRequestsFromFile(path, templt_repo);
    }
}

nemesis::AnimationRequestRepository::AnimationRequestRepository(const std::filesystem::path& data_dir,
                                                                nemesis::TemplateRepository& templt_repo)
{
    AddRequestsFromDirectory(data_dir, templt_repo);
}

void nemesis::AnimationRequestRepository::AddRequest(UPtr<nemesis::AnimationRequest>&& request)
{
    auto& collection = Requests[std::string(request->GetTemplateName())];
    request->SetIndex(collection.size());
    collection.emplace_back(std::move(request));
}

const nemesis::AnimationRequestCollection&
nemesis::AnimationRequestRepository::GetRequests(const std::string& tmplt_name) const
{
    static nemesis::AnimationRequestCollection Proxy;
    auto itr = Requests.find(tmplt_name);
    return itr == Requests.end() ? Proxy : itr->second;
}
