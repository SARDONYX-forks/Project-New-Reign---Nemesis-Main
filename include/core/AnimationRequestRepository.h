#pragma once

#include "core/AnimationRequestCollection.h"

namespace nemesis
{
    struct TemplateRepository;

	struct AnimationRequestRepository
    {
    private:
        UMap<std::string, nemesis::AnimationRequestCollection> Requests;

        void AddRequestsFromFile(const std::filesystem::path& filepath,
                                 nemesis::TemplateRepository& templt_repo);
        void AddRequestsFromDirectory(const std::filesystem::path& dir,
                                      nemesis::TemplateRepository& templt_repo);

    public:
        AnimationRequestRepository() = default;
        AnimationRequestRepository(const std::filesystem::path& data_dir,
                                   nemesis::TemplateRepository& templt_repo);

        void AddRequest(UPtr<nemesis::AnimationRequest>&& request);

        const nemesis::AnimationRequestCollection& GetRequests(const std::string& tmplt_name) const;
    };
}
