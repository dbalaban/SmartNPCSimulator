#ifndef COMMON_INCLUDES_PARAM_READER_HPP
#define COMMON_INCLUDES_PARAM_READER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

typedef std::pair<std::string, std::string> ClassConfigFile;
typedef std::vector<ClassConfigFile> ClassConfigFiles;

class ParamReader {
public:
  ParamReader(const ClassConfigFiles& configFilePath) {
    for (const auto& classConfig : configFilePath) {
      const std::string& className = classConfig.first;
      const std::string& filePath = classConfig.second;
      config[className] = YAML::LoadFile(filePath);
    }
  }
  ~ParamReader() = default;

  

template<typename T>
T getParam(const std::string& className,
                        const std::string& paramName,
                        const T& defaultValue) const {
  auto it = config.find(className);
  if (it != config.end()) {
    const YAML::Node& node = it->second;
    if (node[paramName]) {
      return node[paramName].as<T>(defaultValue);
    }
  }
  return defaultValue;
}

private:
  std::unordered_map<std::string, YAML::Node> config;
};

#endif // COMMON_INCLUDES_PARAM_READER_HPP