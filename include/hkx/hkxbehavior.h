#pragma once

#include "hkx/HkxFile.h"

namespace nemesis
{
    struct SemanticManager;

	struct HkxBehavior : public nemesis::HkxFile
    {
    private:
        UPtr<nemesis::CollectionObject> DataNodes;

        static UPtr<nemesis::CollectionObject> ParseDataNodes(nemesis::LineStream& stream,
                                                              nemesis::SemanticManager& manager,
                                                              Map<std::string, nemesis::HkxNode*>& node_map);
        static UPtr<nemesis::CollectionObject>
        ParseRegularNodes(nemesis::LineStream& stream,
                          nemesis::SemanticManager& manager,
                          Map<std::string, nemesis::HkxNode*>& node_map);

        HkxBehavior() = default;

    public:
        void CompileTo(DeqNstr& lines, nemesis::CompileState& state) const override;
        void SerializeTo(DeqNstr& lines) const override;

        UPtr<nemesis::NObject> CloneNObject() const override;
        UPtr<nemesis::HkxBehavior> Clone() const;

        static UPtr<nemesis::HkxBehavior> ParseFromFile(const std::filesystem::path& filepath);
        static UPtr<nemesis::HkxBehavior> ParseFromFile(const std::filesystem::path& filepath,
                                                        nemesis::ThreadPool& thread_pool);
    };
}
