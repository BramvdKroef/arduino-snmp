#ifndef __SNMP_H__
#define __SNMP_H__

#include "Arduino.h"
#include "ber-decode.h"
#include "ber-encode.h"

/* SNMP PDU context tags */
#define SNMP_GET_REQUEST     (0 | BER_CLASS_CONTEXT | BER_CONSTRUCTED)
#define SNMP_GETNEXT_REQUEST (1 | BER_CLASS_CONTEXT | BER_CONSTRUCTED)
#define SNMP_GET_RESPONSE    (2 | BER_CLASS_CONTEXT | BER_CONSTRUCTED)
#define SNMP_SET_REQUEST     (3 | BER_CLASS_CONTEXT | BER_CONSTRUCTED)
#define SNMP_TRAP            (4 | BER_CLASS_CONTEXT | BER_CONSTRUCTED)

/* SNMP Application types */

// OCTET STRING (size(4))
#define SNMP_IP_ADDR    (0 | BER_CLASS_APPLICATION | BER_PRIMITIVE)
// INTEGER (0..4294967295)
#define SNMP_COUNTER    (1 | BER_CLASS_APPLICATION | BER_PRIMITIVE)
// INTEGER (0..4294967295)
#define SNMP_GAUGE      (2 | BER_CLASS_APPLICATION | BER_PRIMITIVE)
// INTEGER (0..4294967295)
#define SNMP_TIME_TICKS (3 | BER_CLASS_APPLICATION | BER_PRIMITIVE)
// OCTET STRING
#define SNMP_OPAQUE     (4 | BER_CLASS_APPLICATION | BER_PRIMITIVE)

#define SNMP_ERROR_NOERROR    0
#define SNMP_ERROR_TOOBIG     1
#define SNMP_ERROR_NOSUCHNAME 2
#define SNMP_ERROR_BADVALUE   3
#define SNMP_ERROR_READONLY   4
#define SNMP_ERROR_GENERR     5


// Max number of value bindings in a snmp packet
#define SNMP_MAX_VARS 5
#define SNMP_COMMUNITY_MAXLEN 20

typedef struct {
  oid id;
  uint8_t type;
  uint8_t* data;
  size_t size;
} var_bind;

typedef struct {
  uint8_t pdu_type;
  char community[SNMP_COMMUNITY_MAXLEN];
  int32_t request_id;
  int32_t error_status;
  int32_t error_index;
} snmp_packet;

#define SNMP_VERSION 0

int snmp_oid_cmp(oid* oid1, oid* oid2);
size_t snmp_decode(byte* data, int size,
                   var_bind* values, size_t values_len,
                   const char* community);
size_t snmp_decode_pdu(ber_buffer* pdu, snmp_packet* packet,
                       var_bind* varbindlist, size_t varbindlist_len);
size_t snmp_encode_var(var_bind* var, uint8_t* data);
void snmp_decode_getRequest(snmp_packet* request,
                            var_bind* r_vals, size_t r_vals_len,
                            var_bind* values, size_t values_len);
void snmp_decode_getNextRequest(snmp_packet* request,
                                var_bind* r_vals, size_t r_vals_len,
                                var_bind* values, size_t values_len);
size_t snmp_encode_getResponse(uint8_t* response, size_t size,
                               snmp_packet* request,
                               var_bind* bindings, size_t buffer_size);



#endif
