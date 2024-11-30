#ifndef DATA_WRITER_HPP
#define DATA_WRITER_HPP

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>

// data format:
// <isNewColumn - boolean>, <(if isNewColumn) new column header - string>, <columnID - UInt>, <dataType - UInt>, <data value - dataType>
// repeat for each column, new line at end of row
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
    if (file.is_open()) {
      file.close();
    }
    file.open(filename, std::ios::binary | std::ios::out);
    if (!file) {
      throw std::runtime_error("Unable to open file");
    }
  }

  void closeFile() {
    if (file.is_open()) {
      file.close();
    }
  }

  template<typename T>
  void writeData(const std::string& label, const DataType datatype, const T& value) {
    // if no file is open, print error and return
    if (!file.is_open()) {
      std::cerr << "No file open, skipping write of column: " << label << std::endl;
      return;
    }
    if (!verifyTypeMatch(datatype, value)) {
      std::cerr << "Data type does not match value type, skipping write of column: " << label << std::endl;
      return;
    }
    if (columnMap.find(label) == columnMap.end()) {
      columnMap[label] = nextColumnID++;
      writeValue(true); // Column header not seen before
      writeString(label);
    } else {
      writeValue(false); // Column header seen before
    }
    writeValue(columnMap[label]);
    writeValue(static_cast<unsigned int>(datatype));
    if constexpr (std::is_same_v<T, std::string>) {
      writeString(value);
    } else if constexpr (is_vector<T>::value) {
      writeVector(value);
    } else {
      writeValue(value);
    }
  }

  void endLine() {
    file.put('\n');
  }

private:
  DataWriter() : nextColumnID(0) {}
  ~DataWriter() {
    if (file.is_open()) {
      file.close();
    }
  }

  DataWriter(const DataWriter&) = delete;
  DataWriter& operator=(const DataWriter&) = delete;

  template<typename T>
  bool verifyTypeMatch(const DataType datatype, const T& value) {
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
  bool getType(T& value, DataType& datatype) {
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
  void writeValue(T value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeString(const std::string& str) {
    size_t length = str.size();
    writeValue(length);
    file.write(str.c_str(), length);
  }

  template <typename T>
  void writeVector(const T& value) {
    using E = typename vector_element_type<T>::type;
    const std::vector<E>& vec = value;

    size_t length = vec.size();
    writeValue(length);
    if (length == 0) {
      std::cerr << "Empty vector, skipping write" << std::endl;
      return;
    }
    DataType datatype;
    if (!getType(vec[0], datatype)) {
      std::cerr << "Unsupported vector type, skipping write" << std::endl;
      return;
    }
    writeValue(static_cast<unsigned int>(datatype));
    for (const auto& val : vec) {
      if constexpr (std::is_same<E, std::string>::value) {
        writeString(val);
      } else if constexpr (is_vector<E>::value) {
        writeVector(val);
      } else {
        writeValue(val);
      }
    }
  }

  std::ofstream file;
  std::unordered_map<std::string, int> columnMap;
  int nextColumnID;
};

} // namespace data_management

#endif // DATA_WRITER_HPP