#pragma once

#include "animsetdata/state.h"
#include "core/linked.h"
#include "utilities/conditiondetails.h"

namespace nemesis {
namespace animsetdata {
struct LinkedState : nemesis::Linked<nemesis::animsetdata::State,
                                     nemesis::animsetdata::LinkedState> {
 public:
  LinkedState() = default;
  LinkedState(const nemesis::Line& line);

  void GetRawName(VecNstr& lines) const;

  void SetRawDataTo(VecNstr& lines) const;
  void SetNewConditionedDataTo(VecNstr& lines, const nemesis::Line& newline,
                               const LinkedCondition& cond) const override;
  void SetRawConditionedDataTo(VecNstr& lines) const override;
};
}  // namespace animsetdata
}  // namespace nemesis
