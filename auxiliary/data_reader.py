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
          line_size = self.read_value(file, 'I')
          line_data = file.read(line_size)
          if not line_data:
            break
          self.process_line(line_data)
        except EOFError:
          break

  def process_line(self, line_data):
    file = memoryview(line_data)
    offset = 0
    while offset < len(file):
      is_new_column = self.read_value(file, '?', offset)
      offset += struct.calcsize('?')
      if is_new_column:
        column_header = self.read_string(file, offset)
        offset += struct.calcsize('I') + len(column_header)
        column_id = self.read_value(file, 'I', offset)
        offset += struct.calcsize('I')
        self.column_map[column_id] = column_header
      else:
        column_id = self.read_value(file, 'I', offset)
        offset += struct.calcsize('I')

      datatype = DataType(self.read_value(file, 'I', offset))
      offset += struct.calcsize('I')
      value, value_size = self.read_data(file, datatype, offset)
      offset += value_size

      column_header = self.column_map[column_id]
      self.data[column_header].append(value)

  def read_value(self, file, fmt, offset=0):
    size = struct.calcsize(fmt)
    data = file[offset:offset + size]
    if not data:
      raise EOFError
    return struct.unpack(fmt, data)[0]

  def read_string(self, file, offset):
    length = self.read_value(file, 'I', offset)
    offset += struct.calcsize('I')
    data = file[offset:offset + length]
    return data.tobytes().decode('utf-8')

  def read_vector(self, file, offset):
    length = self.read_value(file, 'I', offset)
    offset += struct.calcsize('I')
    if length == 0:
      return [], struct.calcsize('I')
    datatype = DataType(self.read_value(file, 'I', offset))
    offset += struct.calcsize('I')
    vector = []
    total_size = struct.calcsize('I') * 2
    for _ in range(length):
      value, value_size = self.read_data(file, datatype, offset)
      vector.append(value)
      offset += value_size
      total_size += value_size
    return vector, total_size

  def read_data(self, file, datatype, offset):
    if datatype == DataType.BOOLEAN:
      value = self.read_value(file, '?', offset)
      return value, struct.calcsize('?')
    elif datatype == DataType.INT:
      value = self.read_value(file, 'i', offset)
      return value, struct.calcsize('i')
    elif datatype == DataType.UINT:
      value = self.read_value(file, 'I', offset)
      return value, struct.calcsize('I')
    elif datatype == DataType.SIZE:
      value = self.read_value(file, 'Q', offset)
      return value, struct.calcsize('Q')
    elif datatype == DataType.DOUBLE:
      value = self.read_value(file, 'd', offset)
      return value, struct.calcsize('d')
    elif datatype == DataType.STRING:
      value = self.read_string(file, offset)
      return value, struct.calcsize('I') + len(value)
    elif datatype == DataType.VECTOR:
      value, size = self.read_vector(file, offset)
      return value, size
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
  parser.add_argument("output", help="(Optional) Re-Pickled data file", nargs='?', default="")
  args = parser.parse_args()
  reader = DataReader(args.filename)
  if args.output:
    rePickle(reader, args.output)