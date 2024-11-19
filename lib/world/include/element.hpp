#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <cstddef>
#include <type_traits>
#include <memory>

class ElementBase {
public:
  virtual ~ElementBase() = default;

  virtual void update(double elapsedTime) = 0;

  virtual const size_t getElementID() const = 0;

  virtual const size_t getInstanceID() const = 0;

  virtual const size_t getInstanceCount() const = 0;

  virtual const size_t getFeatureSize() const = 0;

  virtual std::unique_ptr<double[]> getFeatures() const = 0;
};

template <class Derived>
class Element : public ElementBase {
public:
  virtual ~Element() = default;

  virtual void update(double elapsedTime) = 0;

  virtual std::unique_ptr<double[]> getFeatures() const = 0;

  // Each derived class must have a const static ID of type size_t
  const size_t getElementID() const override {
    return Derived::ElementID;
  }

  const size_t getInstanceID() const override {
    return instanceID;
  }

  const size_t getInstanceCount() const override {
    return InstanceCount;
  }

  const size_t getFeatureSize() const override {
    return Derived::FeatureSize;
  }

protected:
  static size_t InstanceCount;
  const size_t instanceID;

  Element() : instanceID(InstanceCount++) {}
};

template <class Derived>
size_t Element<Derived>::InstanceCount = 0;

// Static assertion to enforce inheritance
template <class Derived>
struct CheckElementInheritance {
  static_assert(std::is_base_of<Element<Derived>, Derived>::value, "Derived must be derived from Element<Derived>");
};

#endif // ELEMENT_HPP