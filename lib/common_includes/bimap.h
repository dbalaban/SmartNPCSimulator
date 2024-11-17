#ifndef BIMAP_H
#define BIMAP_H

#include <map>
#include <stdexcept>

template <typename KeyType, typename ValueType>
class Bimap {
public:
  void insert(const KeyType& key, const ValueType& value) {
    if (key_to_value.find(key) != key_to_value.end() || value_to_key.find(value) != value_to_key.end()) {
      throw std::runtime_error("Duplicate key or value");
    }
    key_to_value[key] = value;
    value_to_key[value] = key;
  }

  void erase_by_key(const KeyType& key) {
    auto it = key_to_value.find(key);
    if (it != key_to_value.end()) {
      value_to_key.erase(it->second);
      key_to_value.erase(it);
    }
  }

  void erase_by_value(const ValueType& value) {
    auto it = value_to_key.find(value);
    if (it != value_to_key.end()) {
      key_to_value.erase(it->second);
      value_to_key.erase(it);
    }
  }

  ValueType get_value(const KeyType& key) const {
    auto it = key_to_value.find(key);
    if (it == key_to_value.end()) {
      throw std::runtime_error("Key not found");
    }
    return it->second;
  }

  KeyType get_key(const ValueType& value) const {
    auto it = value_to_key.find(value);
    if (it == value_to_key.end()) {
      throw std::runtime_error("Value not found");
    }
    return it->second;
  }

  bool contains_key(const KeyType& key) const {
    return key_to_value.find(key) != key_to_value.end();
  }

  bool contains_value(const ValueType& value) const {
    return value_to_key.find(value) != value_to_key.end();
  }

private:
  std::map<KeyType, ValueType> key_to_value;
  std::map<ValueType, KeyType> value_to_key;
};

#endif // BIMAP_H