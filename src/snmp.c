#include "snmp.h"


int snmp_oid_cmp(oid* oid1, oid* oid2) {
  if (oid1->len != oid2->len)
    return oid1->len - oid2->len;

  return memcmp(oid1->data, oid2->data, oid1->len);
}

size_t snmp_decode(byte* data, int size,
                   var_bind* values, size_t values_len,
                   const char* community) {
  ber_buffer packet, pdu;
  int version;
  snmp_packet snmp;
  var_bind varbindlist[SNMP_MAX_VARS];
  size_t varbindlist_len;

  ber_decode_init(&packet, data, size);
  
  // version
  ber_decode_int(&packet, &version);

  // community
  ber_decode_octet_str(&packet, snmp.community, SNMP_COMMUNITY_MAXLEN);
  if (strcmp(community, snmp.community) != 0)
    return 0;

  snmp.pdu_type = ber_decode_type(&packet);
  ber_decode_sequence(&packet, &pdu);

  varbindlist_len = snmp_decode_pdu(&pdu, &snmp, varbindlist,
                                    SNMP_MAX_VARS);
  
  if (varbindlist_len > SNMP_MAX_VARS) {
    snmp.error_status = SNMP_ERROR_TOOBIG;
    return snmp_encode_getResponse(data, size, &snmp, varbindlist, SNMP_MAX_VARS);
  }
  
  switch(snmp.pdu_type) {
  case SNMP_GET_REQUEST:
    snmp_decode_getRequest(&snmp, varbindlist, varbindlist_len, values, values_len);
    break;
    
  case SNMP_GETNEXT_REQUEST:
    snmp_decode_getNextRequest(&snmp, varbindlist, varbindlist_len, values, values_len);
    break;
    
  case SNMP_SET_REQUEST:
    snmp.error_status = SNMP_ERROR_READONLY;
    break;
    
  default:
    return 0;
  }
  
  return snmp_encode_getResponse(data, size, &snmp, varbindlist, varbindlist_len);
}

size_t snmp_decode_pdu(ber_buffer* pdu, snmp_packet* packet,
                    var_bind* varbindlist, size_t varbindlist_len) {
  ber_buffer varbindings, object;
  size_t i = 0;
  
  ber_decode_int(pdu, &packet->request_id);
  ber_decode_int(pdu, &packet->error_status);
  ber_decode_int(pdu, &packet->error_index);

  ber_decode_sequence(pdu, &varbindings);
  
  while (ber_decode_sequence(&varbindings, &object)) {
    if (i == varbindlist_len) {
      return i + 1;
    }
    
    ber_decode_oid(&object, &varbindlist[i].id);

    varbindlist[i].type = ber_decode_type(&object);
    varbindlist[i].data = object.current;
    varbindlist[i].size = ber_decode_length(&object);
    ber_decode_buffer_next(&object);
    
    i++;
  }
  
  return i;
}

size_t snmp_encode_var(var_bind* var, uint8_t* data) {
  
  switch(var->type) {
  case BER_INTEGER:
  case SNMP_COUNTER:
  case SNMP_GAUGE:
  case SNMP_TIME_TICKS:
  case SNMP_IP_ADDR:
    var->size = ber_encode_integer(var->data, var->type, *(int*)data);
    break;

  case BER_OCTET_STR:
  case SNMP_OPAQUE:
    var->size = ber_encode_str(var->data, var->type, (char*)data);
    break;

  case BER_OBJECT:
    var->size = ber_encode_object(var->data, (oid*)data);
    break;
    
  case BER_NULL:
    var->size = ber_encode_null(var->data);
    
  default:
    return 0;
  }
  return var->size;
}


void snmp_decode_getRequest(snmp_packet* request,
                            var_bind* r_vals, size_t r_vals_len,
                            var_bind* values, size_t values_len) {
  int i, j, cmp;

  for (i = 0; i < r_vals_len; i++) {
    for (j = 0; j < values_len; j++) {
      cmp = snmp_oid_cmp(&values[j].id, &r_vals[i].id);
      if (cmp == 0) {
        r_vals[i] = values[j];
        break;
      } else if (cmp > 0) {
        break;
      }
    }
    if (r_vals[i].type == BER_NULL) {
      request->error_status = SNMP_ERROR_NOSUCHNAME;
      request->error_index = i;
      return;
    }
  }
  return;
}

void snmp_decode_getNextRequest(snmp_packet* request,
                                  var_bind* r_vals, size_t r_vals_len,
                                  var_bind* values, size_t values_len) {
  int i, j, cmp;
  
  for (i = 0; i < r_vals_len; i++) {
    for (j = 0; j < values_len; j++) {
      cmp = snmp_oid_cmp(&values[j].id, &r_vals[i].id);
      if (cmp > 0) {
        r_vals[i] = values[j];
        break;
      }
    }
    if (r_vals[i].type == BER_NULL) {
      request->error_status = SNMP_ERROR_NOSUCHNAME;
      request->error_index = i;
      return;
    }
  }
  return;
}

size_t snmp_encode_getResponse(uint8_t* response, size_t buffer_size,
                               snmp_packet* request,
                               var_bind* bindings, size_t bindings_len) {
  size_t i, response_len, pdu_len, varbindlist_len = 0;
  size_t varbind_len[SNMP_MAX_VARS];

  // Calculate the length of the sequences
  for (i = 0; i < bindings_len; i++) {
    varbind_len[i] = ber_encode_calcHeadLen(bindings[i].id.len);
    varbind_len[i] += bindings[i].id.len;

    varbind_len[i] += ber_encode_calcHeadLen(bindings[i].size);
    varbind_len[i] += bindings[i].size;

    varbindlist_len += ber_encode_calcHeadLen(varbind_len[i]);
    varbindlist_len += varbind_len[i];
  }
  
  pdu_len = ber_encode_calcHeadLen(varbindlist_len);
  pdu_len += 2 + ber_encode_integer_size(request->request_id) + varbindlist_len;
  
  response_len = ber_encode_calcHeadLen(pdu_len);
  response_len += 1 + strlen(request->community) + pdu_len;

  // Encode the data
  response += ber_encode_sequence(response, BER_SEQUENCE, response_len);
  response += ber_encode_integer(response, BER_INTEGER, SNMP_VERSION);
  response += ber_encode_str(response, BER_OCTET_STR, request->community);

  response += ber_encode_sequence(response, SNMP_GET_RESPONSE, pdu_len);
  response += ber_encode_integer(response, BER_INTEGER, request->request_id);
  response += ber_encode_integer(response, BER_INTEGER, request->error_status);
  response += ber_encode_integer(response, BER_INTEGER, request->error_index);

  response += ber_encode_sequence(response, BER_SEQUENCE, varbindlist_len);
  
  for (i = 0; i < bindings_len; i++) {
    response += ber_encode_sequence(response, BER_SEQUENCE, varbind_len[i]);
    
    response += ber_encode_object(response, &bindings[i].id);
    response += ber_encode_octets(response, bindings[i].type,
                                  bindings[i].data,
                                  bindings[i].size);
  }

  return ber_encode_calcHeadLen(response_len) + response_len;
}
