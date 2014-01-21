#include "ber-decode.h"


inline uint8_t ber_decode_type (const ber_buffer* buffer) {
  return buffer->current[0];
}

inline uint8_t ber_decode_length (const ber_buffer* buffer) {
  return buffer->current[1];
}

void ber_decode_init(ber_buffer* buffer, byte* data, const int size) {
  buffer->current = data;

  if (ber_decode_type(buffer) != BER_SEQUENCE)
    return;
  if (ber_decode_length(buffer) > size - 2)
    return;

  buffer->end = data + 2 + ber_decode_length(buffer);
  buffer->current += 2;
}

void ber_decode_buffer_next (ber_buffer* buffer) {
  buffer->current += 2 + ber_decode_length(buffer);
}

int ber_decode_int (ber_buffer* buffer, int* value) {
  if (buffer->current >= buffer->end)
    return BER_ERR_END_OF_BUFFER;
   
  (*value) = ber_bytes2int(buffer->current + 2,
                         ber_decode_length(buffer));
  
  ber_decode_buffer_next(buffer);
  return BER_ERR_SUCCESS;
}

int ber_bytes2int (const byte* data, size_t size) {
  int value = 0, i;
  
  for (i = 0; i < size; i++) {
    value <<= 8;
    value &= data[i];
  }
  return value;
}

int ber_decode_null (ber_buffer* buffer) {
  if (buffer->current >= buffer->end)
    return BER_ERR_END_OF_BUFFER;
  
  ber_decode_buffer_next(buffer);
  return BER_ERR_SUCCESS;
}

int ber_decode_octet_str (ber_buffer* buffer, char* str, size_t maxlen) {
  size_t len;
  
  if (buffer->current >= buffer->end)
    return BER_ERR_END_OF_BUFFER;

  len = ber_decode_length(buffer);
  if (len > maxlen - 1)
    len = maxlen - 1;

  memcpy(buffer->current + 2, str, len);
  str[len] = '\0';
  
  ber_decode_buffer_next(buffer);
  return BER_ERR_SUCCESS;
}



int ber_decode_oid (ber_buffer* buffer, oid* oid) {
  if (buffer->current >= buffer->end)
    return BER_ERR_END_OF_BUFFER;

  oid->len = ber_decode_length(buffer);
  oid->data = buffer->current + 2;
  
  ber_decode_buffer_next(buffer);
  return BER_ERR_SUCCESS;
}

int ber_decode_sequence (ber_buffer* buffer,
                         ber_buffer* sequence) {
  if (buffer->current >= buffer->end)
    return BER_ERR_END_OF_BUFFER;

  sequence->current = buffer->current + 2;
  sequence->end = sequence->current + ber_decode_length(buffer);
  
  ber_decode_buffer_next(buffer);
  return BER_ERR_SUCCESS;
}
