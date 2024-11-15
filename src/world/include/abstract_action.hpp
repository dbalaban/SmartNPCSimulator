#ifndef ABSTRACT_ACTION_HPP
#define ABSTRACT_ACTION_HPP

#include <string>

#include "element.hpp"

struct ActionDesc {
  size_t SubjectClassID;
  size_t SubjectInstanceID;
  size_t ActionID;
  size_t ObjectClassID;
  size_t ObjectInstanceID;
};

template <class Derived>
class AbstractAction {
public:
  AbstractAction() = delete;

  static const size_t getActionID() const {
    return Derived::ActionID;
  }

  static void execute(ElementBase* subject, ElementBase* object) {
    Derived::takeAction(subject, object);
  }
};

#endif // ABSTRACT_ACTION_HPP