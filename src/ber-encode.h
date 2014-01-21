#ifndef __BER_ENCODE_H__
#define __BER_ENCODE_H__

#include "Arduino.h"
#include "ber.h"

size_t ber_encode_calcHeadLen(size_t data_length);
size_t ber_encode_length(byte* data, size_t length);
size_t ber_encode_integer(byte* data, uint8_t type, int value);
size_t ber_encode_integer_size(int value);
size_t ber_encode_octets(uint8_t* data, uint8_t type, uint8_t* value,
                         size_t len);
size_t ber_encode_str(uint8_t* data, uint8_t type, const char* value);
size_t ber_encode_object (byte* data, oid* oid);
size_t ber_encode_null(byte* data);
size_t ber_encode_sequence(byte* data, uint8_t type, size_t size);

#endif
