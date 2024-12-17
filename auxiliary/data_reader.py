import struct
import pickle
import argparse
from collections import defaultdict
from io import BufferedReader
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
  def __init__(self, filename: str):
    self.filename = filename
    self.data = defaultdict(list)
    self.column_map = {}
    self.read_file()

  def read_file(self):
    with open(self.filename, 'rb') as file:
      while True:
        try:
          # Read the size of the next line
          line_size = self.read_value_from_file(file, 'I')
          # Read the line data into a memoryview
          line_data = memoryview(file.read(line_size))
          if not line_data:
            break
          self.process_line(line_data)
        except EOFError:
          break

  def process_line(self, line_data : memoryview):
    offset = 0
    while offset < len(line_data):
      is_new_column = self.read_value(line_data, '?', offset)
      offset += struct.calcsize('?')
      if is_new_column:
        column_header = self.read_string(line_data, offset)
        offset += struct.calcsize('Q') + len(column_header)
        column_id = self.read_value(line_data, 'I', offset)
        offset += struct.calcsize('I')
        self.column_map[column_id] = column_header
      else:
        column_id = self.read_value(line_data, 'I', offset)
        column_header = self.column_map[column_id]
        offset += struct.calcsize('I')

      datatype = DataType(self.read_value(line_data, 'I', offset))
      offset += struct.calcsize('I')
      value, value_size = self.read_data(line_data, datatype, offset)
      offset += value_size

      self.data[column_header].append(value)

  def read_value_from_file(self, file : BufferedReader, fmt : str):
    size = struct.calcsize(fmt)
    data = file.read(size)
    if not data:
      raise EOFError
    return struct.unpack(fmt, data)[0]

  def read_value(self, memview : memoryview, fmt : str, offset : int=0):
    size = struct.calcsize(fmt)
    data = memview[offset:offset + size]
    if not data:
      raise EOFError
    return struct.unpack(fmt, data)[0]

  def read_string(self, memview : memoryview, offset : int):
    length = self.read_value(memview, 'Q', offset)
    offset += struct.calcsize('Q')
    data = memview[offset:offset + length]
    return data.tobytes().decode('utf-8')

  def read_vector(self, memview : memoryview, offset : int):
    length = self.read_value(memview, 'Q', offset)
    total_size = struct.calcsize('Q')
    if length == 0:
      return [], struct.calcsize('Q')
    datatype = DataType(self.read_value(memview, 'I', offset+total_size))
    total_size += struct.calcsize('I')
    vector = []
    for _ in range(length):
      value, value_size = self.read_data(memview, datatype, offset+total_size)
      vector.append(value)
      total_size += value_size
    return vector, total_size

  def read_data(self, memview : memoryview, datatype : DataType, offset : int):
    if datatype == DataType.BOOLEAN:
      value = self.read_value(memview, '?', offset)
      return value, struct.calcsize('?')
    elif datatype == DataType.INT:
      value = self.read_value(memview, 'i', offset)
      return value, struct.calcsize('i')
    elif datatype == DataType.UINT:
      value = self.read_value(memview, 'I', offset)
      return value, struct.calcsize('I')
    elif datatype == DataType.SIZE:
      value = self.read_value(memview, 'Q', offset)
      return value, struct.calcsize('Q')
    elif datatype == DataType.DOUBLE:
      value = self.read_value(memview, 'd', offset)
      return value, struct.calcsize('d')
    elif datatype == DataType.STRING:
      value = self.read_string(memview, offset)
      return value, struct.calcsize('I') + len(value)
    elif datatype == DataType.VECTOR:
      value, size = self.read_vector(memview, offset)
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
  parser.add_argument("--filename", required=True, help="Datafile to read")
  parser.add_argument("--output", help="(Optional) Re-Pickled data file", default="")
  args = parser.parse_args()
  reader = DataReader(args.filename)
  if args.output:
    rePickle(reader, args.output)