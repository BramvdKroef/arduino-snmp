#ifndef __BER_DECODE_H__
#define __BER_DECODE_H__

#include "Arduino.h"
#include "ber.h"

#define BER_ERR_SUCCESS       0
#define BER_ERR_END_OF_BUFFER 1

typedef struct {
uint8_t* current;
const uint8_t* end;
} ber_buffer;

inline uint8_t ber_decode_type (const ber_buffer* buffer);
inline uint8_t ber_decode_length (const ber_buffer* buffer);
void ber_decode_init(ber_buffer* buffer, byte* data, const int size);
void ber_decode_buffer_next (ber_buffer* buffer);
int ber_decode_int (ber_buffer* buffer, int* value);
int ber_bytes2int (const byte* data, size_t size);
int ber_decode_null (ber_buffer* buffer);
int ber_decode_octet_str (ber_buffer* buffer, char* str, size_t maxlen);
int ber_decode_oid (ber_buffer* buffer, oid* oid);
int ber_decode_sequence (ber_buffer* buffer, ber_buffer* sequence);

#endif
