#ifndef UNORDERED_BIMAP_H
#define UNORDERED_BIMAP_H

#include <unordered_map>
#include <stdexcept>

template<typename KeyType, typename ValueType>
class UnorderedBimap {
public:
  void insert(const KeyType& key, const ValueType& value) {
    if (key_to_value.find(key) != key_to_value.end() || value_to_key.find(value) != value_to_key.end()) {
      throw std::invalid_argument("Duplicate key or value");
    }
    key_to_value[key] = value;
    value_to_key[value] = key;
  }

  ValueType get_value(const KeyType& key) const {
    auto it = key_to_value.find(key);
    if (it == key_to_value.end()) {
      throw std::out_of_range("Key not found");
    }
    return it->second;
  }

  KeyType get_key(const ValueType& value) const {
    auto it = value_to_key.find(value);
    if (it == value_to_key.end()) {
      throw std::out_of_range("Value not found");
    }
    return it->second;
  }

  void erase_by_key(const KeyType& key) {
    auto it = key_to_value.find(key);
    if (it == key_to_value.end()) {
      throw std::out_of_range("Key not found");
    }
    value_to_key.erase(it->second);
    key_to_value.erase(it);
  }

  void erase_by_value(const ValueType& value) {
    auto it = value_to_key.find(value);
    if (it == value_to_key.end()) {
      throw std::out_of_range("Value not found");
    }
    key_to_value.erase(it->second);
    value_to_key.erase(it);
  }

  bool contains_key(const KeyType& key) const {
    return key_to_value.find(key) != key_to_value.end();
  }

  bool contains_value(const ValueType& value) const {
    return value_to_key.find(value) != value_to_key.end();
  }

private:
  std::unordered_map<KeyType, ValueType> key_to_value;
  std::unordered_map<ValueType, KeyType> value_to_key;
};

#endif // UNORDERED_BIMAP_H