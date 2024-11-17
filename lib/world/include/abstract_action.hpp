#ifndef ABSTRACT_ACTION_HPP
#define ABSTRACT_ACTION_HPP

#include <cstddef>
#include <unordered_map>
#include <functional>
#include <stdexcept>

struct ActionDesc {
  size_t SubjectClassID;
  size_t SubjectInstanceID;
  size_t ActionID;
  size_t ObjectClassID;
  size_t ObjectInstanceID;
  ElementBase* subject;
  ElementBase* object;
};

class AbstractAction {
public:
  using ActionFunction = std::function<void(ElementBase*, ElementBase*)>;

  AbstractAction() = delete;

  static void registerAction(size_t actionID, ActionFunction func) {
    getRegistry()[actionID] = func;
  }

  static void execute(size_t actionID, ElementBase* subject, ElementBase* object) {
    auto& registry = getRegistry();
    auto it = registry.find(actionID);
    if (it != registry.end()) {
      it->second(subject, object);
    } else {
      throw std::runtime_error("Action ID not found in registry");
    }
  }

private:
  static std::unordered_map<size_t, ActionFunction>& getRegistry() {
    static std::unordered_map<size_t, ActionFunction> registry;
    return registry;
  }
};

#endif // ABSTRACT_ACTION_HPP