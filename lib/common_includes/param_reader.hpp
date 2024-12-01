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

typedef std::unordered_map<std::string, std::string> ClassConfig;
typedef std::unique_ptr<ClassConfig> ClassConfigPtr;
typedef std::unordered_map<std::string, ClassConfigPtr> Config;
typedef std::unique_ptr<Config> ConfigPtr;

class ParamReader {
public:
  void addConfigFiles(const ClassConfigFiles& configFilePath) {
    for (const auto& classConfig : configFilePath) {
      const std::string& className = classConfig.first;
      const std::string& filePath = classConfig.second;
      ClassConfigPtr classConfigPtr = std::make_unique<ClassConfig>();
      if (loadYamlFile(filePath, classConfigPtr)) {
        (*config)[className] = std::move(classConfigPtr);
      } else {
        std::cerr << "Failed to load config file " << filePath << std::endl;
      }
    }
  }

  static ParamReader& getInstance() {
    static ParamReader instance;
    return instance;
  }

  template<typename T>
  T getParam(const std::string& className,
             const std::string& paramName,
             const T& defaultValue) const {
    if (config->count(className) > 0) {
      const ClassConfigPtr& node = config->at(className);
      if (node->count(paramName) > 0) {
        return convert<T>(node->at(paramName));
      } else {
        std::cerr << "Param " << paramName << " not found in class " << className << "!" << std::endl;
      }
    } else {
      std::cerr << "Class " << className << " not found in config!" << std::endl;
    }
    return defaultValue;
  }

private:
  ConfigPtr config;

  ParamReader() : config(std::make_unique<Config>()) {}
  ~ParamReader() = default;

  ParamReader(const ParamReader&) = delete;
  ParamReader& operator=(const ParamReader&) = delete;

  bool loadYamlFile(const std::string& filePath, ClassConfigPtr& result) const {
    FILE* file = fopen(filePath.c_str(), "r");
    if (!file) {
      std::cerr << "Failed to open file: " << filePath << std::endl;
      return false;
    }

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser)) {
      std::cerr << "Failed to initialize parser!" << std::endl;
      fclose(file);
      return false;
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
          (*result)[key] = value;
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

    return true;
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