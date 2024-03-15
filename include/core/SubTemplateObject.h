#pragma once

#include "core/IfObject.h"
#include "core/ModObject.h"
#include "core/ForEachObject.h"
#include "core/CollectionObject.h"

namespace nemesis
{
	struct SubTemplateObject : public nemesis::NObject
    {
    private:
        std::string Name;
        UPtr<nemesis::CollectionObject> Data;

    public:
        void CompileTo(DeqNstr& lines, nemesis::CompileState& state) const override;
        void SerializeTo(DeqNstr& lines) const override;

        UPtr<nemesis::NObject> CloneNObject() const override;
        UPtr<nemesis::SubTemplateObject> Clone() const;

        const std::string& GetName() const noexcept;

        static UPtr<nemesis::SubTemplateObject> ParseFromFile(const std::filesystem::path& filepath);
    };
}
