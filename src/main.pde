#include "Arduino.h"
extern "C" {
#include "snmp.h"
}

#define SNMP_VALUES_LENGTH 1
#define SNMP_COMMUNITY "public"

var_bind snmp_values[SNMP_VALUES_LENGTH];
uint8_t test_oid[9] = {0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x01, 0x05, 0x00};
uint8_t snmp_test_value[4];

uint8_t test_packet[44] =
  {0x30, 0x29, // sequence
   0x02, 0x01, 0x00, // version
   0x04, 0x06, 'p', 'u', 'b', 'l', 'i', 'c', // community
   0xa0, 0x1c, // get-request
   0x02, 0x04, 0x00, 0x00, 0x00, 0x01, // request id
   0x02, 0x01, 0x00, // error status
   0x02, 0x01, 0x00, // error index
   0x30, 0x0e, // list sequence
   0x30, 0x0c, // oid/value sequence
   0x06, 0x09, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x01, 0x05, 0x00, //oid
   0x05, 0x00 // null
  } ;
   
void setup () {
  int test_init_value = 42;
  size_t len, i;
  
  snmp_values[0].id.data = test_oid;
  snmp_values[0].id.len = 9;
  snmp_values[0].type = SNMP_GAUGE;
  snmp_values[0].data = snmp_test_value;
  snmp_encode_var(&snmp_values[0], (uint8_t*)&test_init_value);

  len = snmp_decode(test_packet, 44, snmp_values, 1, SNMP_COMMUNITY);
  Serial.begin(9600);
  Serial.println(len);
  for (i = 0; i < len; i++) {
    if (test_packet[i] < 0x10)
      Serial.print("0");
    Serial.print(test_packet[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void loop () {
}
