#ifndef DATA_WRITER_HPP
#define DATA_WRITER_HPP

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

// data format:
// <isNewColumn - boolean>, <(if isNewColumn) new column header - string>, <columnID - UInt>, <dataType - UInt>, <data value - dataType>
// repeat for each column, new line at end of row
// format for string data: <string length - UInt>, <string data - char arr>

namespace data_management {

enum class DataType {
  BOOLEAN,
  INT,
  UINT,
  DOUBLE,
  STRING
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
  bool verifyTypeMatch(const DataType datatype, const T& value) {
    switch (datatype) {
      case DataType::BOOLEAN:
        return std::is_same<T, bool>::value;
      case DataType::INT:
        return std::is_same<T, int>::value;
      case DataType::UINT:
        return std::is_same<T, unsigned int>::value;
      case DataType::DOUBLE:
        return std::is_same<T, double>::value;
      case DataType::STRING:
        return std::is_same<T, std::string>::value;
      default:
        return false;
    }
  }

  template<typename T>
  void writeData(const std::string& label, const DataType datatype, const T& value) {
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
    writeValue(value);
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

  template <typename T>
  void writeValue(T value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeString(const std::string& str) {
    size_t length = str.size();
    writeValue(length);
    file.write(str.c_str(), length);
  }

  std::ofstream file;
  std::unordered_map<std::string, int> columnMap;
  int nextColumnID;
};

} // namespace data_management

#endif // DATA_WRITER_HPP