import struct
import pickle
import argparse
from collections import defaultdict
from enum import Enum

class DataType(Enum):
  BOOLEAN = 0
  INT = 1
  UINT = 2
  SIZE = 3
  DOUBLE = 4
  STRING = 5
  VECTOR = 6

class DataReader:
  def __init__(self, filename):
    self.filename = filename
    self.data = defaultdict(list)
    self.column_map = {}
    self.read_file()

  def read_file(self):
    with open(self.filename, 'rb') as file:
      while True:
        try:
          is_new_column = self.read_value(file, '?')
          if is_new_column:
            column_header = self.read_string(file)
            column_id = self.read_value(file, 'I')
            self.column_map[column_id] = column_header
          else:
            column_id = self.read_value(file, 'I')
          
          datatype = DataType(self.read_value(file, 'I'))
          value = self.read_data(file, datatype)
          
          column_header = self.column_map[column_id]
          self.data[column_header].append(value)
          
          if file.peek(1)[:1] == b'\n':
            file.read(1)  # consume newline
        except EOFError:
          break

  def read_value(self, file, fmt):
    size = struct.calcsize(fmt)
    data = file.read(size)
    if not data:
      raise EOFError
    return struct.unpack(fmt, data)[0]

  def read_string(self, file):
    length = self.read_value(file, 'I')
    data = file.read(length)
    return data.decode('utf-8')

  def read_vector(self, file):
    length = self.read_value(file, 'I')
    if length == 0:
      return []
    datatype = DataType(self.read_value(file, 'I'))
    return [self.read_data(file, datatype) for _ in range(length)]

  def read_data(self, file, datatype):
    if datatype == DataType.BOOLEAN:
      return self.read_value(file, '?')
    elif datatype == DataType.INT:
      return self.read_value(file, 'i')
    elif datatype == DataType.UINT:
      return self.read_value(file, 'I')
    elif datatype == DataType.SIZE:
      return self.read_value(file, 'Q')
    elif datatype == DataType.DOUBLE:
      return self.read_value(file, 'd')
    elif datatype == DataType.STRING:
      return self.read_string(file)
    elif datatype == DataType.VECTOR:
      return self.read_vector(file)
    else:
      raise ValueError(f"Unsupported data type: {datatype}")

def rePickle(reader, filename):
  reader.read_file()
  with open(filename, 'wb') as file:
    pickle.dump(reader.data, file)

# Example usage
if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("filename", help="Datafile to read")
  parser.add_argument("output", help="(Optional) Re-Pickled data file", default="")
  args = parser.parse_args()
  reader = DataReader(args.filename)
  if args.output:
    rePickle(reader, args.output)