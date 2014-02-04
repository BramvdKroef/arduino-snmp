#include "ber-encode.h"

size_t ber_encode_calcHeadLen(const size_t data_length) {
  return 2;
}

size_t ber_encode_length(uint8_t* data, const size_t length) {
  data[0] = (uint8_t)length;
  return 1;
}

size_t ber_encode_integer(uint8_t* data, const uint8_t type, const int32_t value) {
  size_t len, offset, i;
  uint8_t* tmp = &value;

  data[0] = type;

  len = ber_encode_integer_size(value);
  
  offset = 1 + ber_encode_length(data + 1, len);
    
  // copy the bytes from value to data backwards
  for (i = 0; i < len; i++) {
    data[offset + i] = tmp[len - (i + 1)];
  }
  
  return offset + len;
}

size_t ber_encode_integer_size(const int32_t value) {
  size_t len = 1;
  int32_t tmp = value;
  tmp = tmp >> 8;
  // how many bytes are used in value
  while (tmp != 0) {
    len++;
    tmp = tmp >> 8;
  }
  return len;
}

size_t ber_encode_octets(uint8_t* data, const uint8_t type, const uint8_t* value,
                         const size_t len) {
  size_t offset;
  data[0] = type;
  offset = 1 + ber_encode_length(data + 1, len);
  memcpy(data + offset, value, len);
  return offset + len;
}

size_t ber_encode_str(uint8_t* data, const uint8_t type, const char* value) {
  return ber_encode_octets(data, type, (uint8_t*)value, strlen(value));
}

size_t ber_encode_object (uint8_t* data, const oid* oid) {
  return ber_encode_octets(data, BER_OBJECT, oid->data, oid->len);
}

size_t ber_encode_null(uint8_t* data) {
  data[0] = BER_NULL;
  data[1] = 0;
  return 2;
}

size_t ber_encode_sequence(uint8_t* data, const uint8_t type, const size_t size) {
  data[0] = type;
  return 1 + ber_encode_length(data + 1, size);
}

