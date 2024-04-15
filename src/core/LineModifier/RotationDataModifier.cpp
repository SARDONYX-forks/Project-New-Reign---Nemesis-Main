#include "core/LineModifier/RotationDataModifier.h"

nemesis::RotationDataModifier::RotationDataModifier(size_t begin,
                                                    size_t end,
                                                    const std::string& expression,
                                                    size_t linenum,
                                                    const std::filesystem::path& filepath,
                                                    const nemesis::SemanticManager& manager)
    : nemesis::StandardLineModifier<nemesis::RotationDataStatement>(begin, end, expression, linenum, filepath, manager)
{
}
