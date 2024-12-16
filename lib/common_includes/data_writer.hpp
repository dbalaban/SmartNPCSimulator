#ifndef DATA_WRITER_HPP
#define DATA_WRITER_HPP

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;

// data format:
// <line size - UInt> [<isNewColumn - boolean>, <(if isNewColumn) new column header - string>, <columnID - UInt>, <dataType - UInt>, <data value - dataType>]
// repeat [] for each column, new line at end of row
// format for string data: <string length - UInt>, <string data - char arr>
// format for vector data: <vector length - UInt>, <dataType - UInt>, <vector data - dataType>
// if the vector size is 0, only the length is written
// recursive for nested vectors

namespace data_management {

enum class DataType {
  BOOLEAN,
  INT,
  UINT,
  SIZE,
  DOUBLE,
  STRING,
  VECTOR
};

template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : std::true_type {};

// Type trait to extract the element type from a std::vector
template<typename T>
struct vector_element_type {};

template<typename E>
struct vector_element_type<std::vector<E>> {
  using type = E;
};

class DataWriter {
public:
  static DataWriter& getInstance() {
    static DataWriter instance;
    return instance;
  }

  void openFile(const std::string& filename) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (file.is_open()) {
      std::cerr << "File already open, closing before opening new file" << std::endl;
      closeFile();
    }

    std::string finalFilename = filename;
    int counter = 1;
    while (fs::exists(finalFilename)) {
      finalFilename = filename.substr(0, filename.find_last_of('.')) + "_" + std::to_string(counter) + filename.substr(filename.find_last_of('.'));
      counter++;
    }

    if (finalFilename != filename) {
      std::cerr << "Warning: File already exists. Using " << finalFilename << " instead." << std::endl;
    }

    file.open(finalFilename, std::ios::binary | std::ios::out);
    if (!file) {
      throw std::runtime_error("Unable to open file");
    }

    tmpFilename = finalFilename + ".tmp";
    tmpFile.open(tmpFilename, std::ios::binary | std::ios::out);
    if (!tmpFile) {
      throw std::runtime_error("Unable to open temporary file");
    }
  }

  void closeFile() {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (file.is_open()) {
      endLine();
      file.close();
    }
    if (tmpFile.is_open()) {
      tmpFile.close();
      fs::remove(tmpFilename);
    }
  }

  template<typename T>
  void writeData(const char* label, const DataType datatype, const T& value) {
    std::string strLabel(label);
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (!tmpFile.is_open()) {
      std::cerr << "No temporary file open, skipping write of column: " << strLabel << std::endl;
      return;
    }
    if (!verifyTypeMatch(datatype, value)) {
      std::cerr << "Data type does not match value type, skipping write of column: " << strLabel << std::endl;
      return;
    }
    if (columnMap.find(strLabel) == columnMap.end()) {
      columnMap[strLabel] = nextColumnID;
      nextColumnID++;
      writeValue(tmpFile, true); // Column header not seen before
      writeString(tmpFile, strLabel);
    } else {
      writeValue(tmpFile, false); // Column header seen before
    }
    writeValue(tmpFile, columnMap[strLabel]);
    writeValue(tmpFile, static_cast<unsigned int>(datatype));
    if constexpr (std::is_same_v<T, std::string>) {
      writeString(tmpFile, value);
    } else if constexpr (is_vector<T>::value) {
      writeVector(tmpFile, value);
    } else {
      writeValue(tmpFile, value);
    }
  }

  void endLine() {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (!tmpFile.is_open()) {
      return;
    }

    tmpFile.flush();
    tmpFile.seekp(0, std::ios::end);
    std::streampos tmpFileSize = tmpFile.tellp();

    if (tmpFileSize > 0) {
      tmpFile.close();

      // Write the size of the line to the main file
      writeValue(file, static_cast<unsigned int>(tmpFileSize));

      // Transfer data from the temporary file to the main file in chunks
      std::ifstream tmpFileIn(tmpFilename, std::ios::binary);
      if (!tmpFileIn) {
        throw std::runtime_error("Unable to open temporary file for reading");
      }

      const size_t bufferSize = 4096; // 4 KB buffer
      char buffer[bufferSize];

      while (tmpFileIn.read(buffer, bufferSize) || tmpFileIn.gcount() > 0) {
        file.write(buffer, tmpFileIn.gcount());
      }

      tmpFileIn.close();

      // Reopen the temporary file for the next line, clearing its contents
      tmpFile.open(tmpFilename, std::ios::binary | std::ios::out | std::ios::trunc);
      if (!tmpFile) {
        throw std::runtime_error("Unable to reopen temporary file");
      }
    }
  }

private:
  DataWriter() : nextColumnID(0) {}
  ~DataWriter() {
    closeFile();
  }

  DataWriter(const DataWriter&) = delete;
  DataWriter& operator=(const DataWriter&) = delete;

  template<typename T>
  bool verifyTypeMatch(const DataType datatype, const T& value) const {
    switch (datatype) {
      case DataType::BOOLEAN:
        return std::is_same<T, bool>::value;
      case DataType::INT:
        return std::is_same<T, int>::value;
      case DataType::UINT:
        return std::is_same<T, unsigned int>::value;
      case DataType::SIZE:
        return std::is_same<T, size_t>::value;
      case DataType::DOUBLE:
        return std::is_same<T, double>::value;
      case DataType::STRING:
        return std::is_same<T, std::string>::value;
      case DataType::VECTOR:
        return is_vector<T>::value;
      default:
        return false;
    }
  }

  template<typename T>
  bool getType(const T& value, DataType& datatype) const {
    if (std::is_same<T, bool>::value) {
      datatype = DataType::BOOLEAN;
    } else if (std::is_same<T, int>::value) {
      datatype = DataType::INT;
    } else if (std::is_same<T, unsigned int>::value) {
      datatype = DataType::UINT;
    } else if (std::is_same<T, size_t>::value) {
      datatype = DataType::SIZE;
     } else if (std::is_same<T, double>::value) {
      datatype = DataType::DOUBLE;
    } else if (std::is_same<T, std::string>::value) {
      datatype = DataType::STRING;
    } else if (is_vector<T>::value) {
      datatype = DataType::VECTOR;
    } else {
      return false;
    }
    return true;
  }

  template <typename T>
  void writeValue(std::ofstream& outFile, const T& value) {
    outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeString(std::ofstream& outFile, const std::string& str) {
    size_t length = str.size();
    writeValue(outFile, length);
    outFile.write(str.c_str(), length);
  }

  template <typename T>
  void writeVector(std::ofstream& outFile, const T& value) {
    using E = typename vector_element_type<T>::type;
    const std::vector<E>& vec = value;

    size_t length = vec.size();
    writeValue(outFile, length);
    if (length == 0) {
      std::cerr << "Empty vector, skipping write" << std::endl;
      return;
    }
    DataType datatype;
    if (!getType(vec[0], datatype)) {
      std::cerr << "Unsupported vector type, skipping write" << std::endl;
      return;
    }
    writeValue(outFile, static_cast<unsigned int>(datatype));
    for (const auto& val : vec) {
      if constexpr (std::is_same<E, std::string>::value) {
        writeString(outFile, val);
      } else if constexpr (is_vector<E>::value) {
        writeVector(outFile, val);
      } else {
        writeValue(outFile, val);
      }
    }
  }

  std::ofstream file;
  std::ofstream tmpFile;
  std::string tmpFilename;
  std::unordered_map<std::string, uint32_t> columnMap;
  uint32_t nextColumnID;
  mutable std::recursive_mutex mutex;
};

} // namespace data_management

#endif // DATA_WRITER_HPP