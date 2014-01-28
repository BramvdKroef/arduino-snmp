#include "ber-encode.h"

size_t ber_encode_calcHeadLen(size_t data_length) {
  return 2;
}

size_t ber_encode_length(byte* data, size_t length) {
  data[0] = (uint8_t)length;
  return 1;
}

size_t ber_encode_integer(byte* data, uint8_t type, int value) {
  size_t len, offset;
  int i;

  data[0] = type;

  len = ber_encode_integer_size(value);
  
  offset = 1 + ber_encode_length(data + 1, len);
    
  // copy the bytes from value to data backwards
  for (i = len - 1; i >= 0; i--) {
    data[offset + i] = value & 0xFF;
    value >>= 8;
  }
  
  return offset + len;
}

size_t ber_encode_integer_size(int value) {
  size_t len = 1;
  int tmp = value >> 8;
  // how many bytes are used in value
  while (tmp != 0) {
    len++;
    tmp = tmp >> 8;
  }
  return len;
}

size_t ber_encode_octets(uint8_t* data, uint8_t type, uint8_t* value,
                         size_t len) {
  size_t offset;
  data[0] = type;
  offset = 1 + ber_encode_length(data + 1, len);
  memcpy(data + offset, value, len);
  return offset + len;
}

size_t ber_encode_str(uint8_t* data, uint8_t type, const char* value) {
  return ber_encode_octets(data, type, (uint8_t*)value, strlen(value));
}

size_t ber_encode_object (byte* data, oid* oid) {
  return ber_encode_octets(data, BER_OBJECT, oid->data, oid->len);
}

size_t ber_encode_null(byte* data) {
  data[0] = BER_NULL;
  data[1] = 0;
  return 2;
}

size_t ber_encode_sequence(byte* data, uint8_t type, size_t size) {
  data[0] = type;
  return 1 + ber_encode_length(data + 1, size);
}

