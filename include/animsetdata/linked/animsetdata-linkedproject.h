#pragma once

#include "animsetdata/animsetdata-project.h"
#include "core/linked.h"
#include "utilities/conditiondetails.h"

namespace nemesis {
namespace animsetdata {
struct LinkedProject : nemesis::Linked<nemesis::animsetdata::Project,
                                       nemesis::animsetdata::LinkedProject> {
 public:
  LinkedProject() = default;
  LinkedProject(const nemesis::Line& line);

  void GetRawName(VecNstr& lines) const;

  void SetRawDataTo(VecNstr& lines) const;
  void SetNewConditionedDataTo(VecNstr& lines, const nemesis::Line& newline,
                               const LinkedCondition& cond) const override;
  void SetRawConditionedDataTo(VecNstr& lines) const override;
};
}  // namespace animsetdata
}  // namespace nemesis
