#ifndef __BER_ENCODE_H__
#define __BER_ENCODE_H__

#include "Arduino.h"
#include "ber.h"

size_t ber_encode_calcHeadLen(const size_t data_length);
size_t ber_encode_length(uint8_t* data, const size_t length);
size_t ber_encode_integer(uint8_t* data, const uint8_t type, const int32_t value);
size_t ber_encode_integer_size(const int32_t value);
size_t ber_encode_octets(uint8_t* data, const uint8_t type, const uint8_t* value,
                         const size_t len);
size_t ber_encode_str(uint8_t* data, const uint8_t type, const char* value);
size_t ber_encode_object (uint8_t* data, const oid* oid);
size_t ber_encode_null(uint8_t* data);
size_t ber_encode_sequence(uint8_t* data, const uint8_t type, const size_t size);

#endif
