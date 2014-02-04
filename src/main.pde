#include "Arduino.h"
extern "C" {
#include "snmp.h"
}

#define SNMP_VALUES_LENGTH 3
#define SNMP_COMMUNITY "public"

var_bind snmp_values[SNMP_VALUES_LENGTH];


uint8_t sysDescr_oid[]    = {SNMP_OID_SYSTEM, 1, 0};
uint8_t sysDescr_value[6];

uint8_t sysObjectID_oid[] = {SNMP_OID_SYSTEM, 2, 0};
uint8_t sysObjectID_value[8];

uint8_t sysUpTime_oid[]   = {SNMP_OID_SYSTEM, 3, 0};
uint8_t sysUpTime_value[6];

uint8_t vendorOid[] = {SNMP_OID_ENTERPRISE, 99};

size_t snmp_debug(byte* data, int size,
                  var_bind* values, size_t values_len,
                  const char* community);

void hexDump(uint8_t* data, size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if (data[i] < 0x10)
      Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}
uint8_t test_packet[] =
  {0x30, 0x29, // sequence
   0x02, 0x01, 0x00, // version
   0x04, 0x06, 'p', 'u', 'b', 'l', 'i', 'c', // community
   0xa0, 0x1c, // get-request
   0x02, 0x04, 0x00, 0x00, 0x00, 0x01, // request id
   0x02, 0x01, 0x00, // error status
   0x02, 0x01, 0x00, // error index
   0x30, 0x0e, // list sequence
   0x30, 0x0c, // oid/value sequence
   0x06, 0x09, SNMP_OID_SYSTEM, 0x01, 0x00, //oid
   0x05, 0x00 // null
  } ;

void setup () {
  const char* sysDescr = "test";
  oid sysOid;
  uint32_t uptime = 0;
  
  size_t len;

  snmp_init_varbind(&snmp_values[0],
                    sysDescr_oid, sizeof(sysDescr_oid),
                    BER_OCTET_STR, sysDescr_value,
                    (uint8_t*)sysDescr);

  sysOid.data = vendorOid;
  sysOid.len = 6;
  
  snmp_init_varbind(&snmp_values[1],
                    sysObjectID_oid, sizeof(sysObjectID_oid),
                    BER_OBJECT, sysObjectID_value,
                    (uint8_t*)&sysOid);

  snmp_init_varbind(&snmp_values[2],
                    sysUpTime_oid, sizeof(sysUpTime_oid),
                    SNMP_TIME_TICKS, sysUpTime_value,
                    (uint8_t*)&uptime);


  len = snmp_decode(test_packet, sizeof(test_packet), snmp_values, 1, SNMP_COMMUNITY);

  Serial.begin(9600);

  Serial.println(len);
  hexDump(test_packet, len);
}

void loop () {
  uint32_t uptime;
  
  delay(100);
  uptime = millis() / 10;
  snmp_encode_var(&snmp_values[2], (uint8_t*)&uptime);
}

