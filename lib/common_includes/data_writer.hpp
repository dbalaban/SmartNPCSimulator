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
  void writeData(const std::string& label, const DataType datatype, const T& value) {
    if (columnMap.find(label) == columnMap.end()) {
      columnMap[label] = nextColumnID++;
      writeBoolean(false); // Column header not seen before
      writeString(label);
    } else {
      writeBoolean(true); // Column header seen before
    }
    writeUInt(columnMap[label]);
    writeUInt(static_cast<unsigned int>(datatype));
    switch (datatype) {
    case DataType::BOOLEAN:
      bool bValue = static_cast<bool>(value);
      writeBoolean(bValue);
      break;
    case DataType::INT:
      int iValue = static_cast<int>(value);
      writeInt(iValue);
      break;
    case DataType::UINT:
      unsigned int uValue = static_cast<unsigned int>(value);
      writeUInt(uValue);
      break;
    case DataType::DOUBLE:
      double dValue = static_cast<double>(value);
      writeDouble(dValue);
      break;
    case DataType::STRING:
      std::string sValue = static_cast<std::string>(value);
      writeString(sValue);
      break;
    default:
      std::cerr << "Writing Unknown data type, file corrupted" << std::endl;
      break;
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

  void writeBoolean(bool value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeInt(int value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeDouble(double value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeUInt(unsigned int value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  void writeString(const std::string& str) {
    size_t length = str.size();
    writeUInt(length);
    file.write(str.c_str(), length);
  }

  std::ofstream file;
  std::unordered_map<std::string, int> columnMap;
  int nextColumnID;
};

} // namespace data_management

#endif // DATA_WRITER_HPP