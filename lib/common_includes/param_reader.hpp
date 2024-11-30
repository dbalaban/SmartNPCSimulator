#ifndef COMMON_INCLUDES_PARAM_READER_HPP
#define COMMON_INCLUDES_PARAM_READER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <yaml.h>
#include <fstream>
#include <iostream>
#include <sstream>

typedef std::pair<std::string, std::string> ClassConfigFile;
typedef std::vector<ClassConfigFile> ClassConfigFiles;

namespace data_management {

class ParamReader {
public:
  ParamReader(const ClassConfigFiles& configFilePath) {
    for (const auto& classConfig : configFilePath) {
      const std::string& className = classConfig.first;
      const std::string& filePath = classConfig.second;
      config[className] = loadYamlFile(filePath);
    }
  }
  ~ParamReader() = default;

  template<typename T>
  T getParam(const std::string& className,
             const std::string& paramName,
             const T& defaultValue) const {
    auto it = config.find(className);
    if (it != config.end()) {
      const auto& node = it->second;
      auto paramIt = node.find(paramName);
      if (paramIt != node.end()) {
        return convert<T>(paramIt->second);
      }
    }
    return defaultValue;
  }

private:
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config;

  std::unordered_map<std::string, std::string> loadYamlFile(const std::string& filePath) const {
    std::unordered_map<std::string, std::string> result;
    FILE* file = fopen(filePath.c_str(), "r");
    if (!file) {
      std::cerr << "Failed to open file: " << filePath << std::endl;
      return result;
    }

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser)) {
      std::cerr << "Failed to initialize parser!" << std::endl;
      fclose(file);
      return result;
    }

    yaml_parser_set_input_file(&parser, file);

    std::string key;
    bool readingKey = true;

    while (true) {
      if (!yaml_parser_parse(&parser, &event)) {
        std::cerr << "Parser error " << parser.error << std::endl;
        break;
      }

      if (event.type == YAML_SCALAR_EVENT) {
        std::string value(reinterpret_cast<const char*>(event.data.scalar.value));
        if (readingKey) {
          key = value;
          readingKey = false;
        } else {
          result[key] = value;
          readingKey = true;
        }
      }

      if (event.type == YAML_STREAM_END_EVENT) {
        yaml_event_delete(&event);
        break;
      }

      yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);

    return result;
  }

  template<typename T>
  T convert(const std::string& value) const {
    std::istringstream iss(value);
    T result;
    iss >> result;
    return result;
  }

  };

  // Explicit specialization for std::string
  template<>
  inline std::string ParamReader::convert<std::string>(const std::string& value) const {
    return value;
  }

} // namespace data_management

#endif // COMMON_INCLUDES_PARAM_READER_HPP