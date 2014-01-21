#ifndef __BER_H__
#define __BER_H__

#include "Arduino.h"

/* type class */
#define BER_CLASS_UNIVERSAL   0
#define BER_CLASS_APPLICATION (1 << 6)
#define BER_CLASS_CONTEXT     (2 << 6)
#define BER_CLASS_PRIVATE     (3 << 6)

/* P/C type */
#define BER_PRIMITIVE   0
#define BER_CONSTRUCTED (1 << 5)

#define BER_INTEGER    (2 | BER_CLASS_UNIVERSAL | BER_PRIMITIVE)
#define BER_OCTET_STR  (4 | BER_CLASS_UNIVERSAL | BER_PRIMITIVE)
#define BER_NULL       (5 | BER_CLASS_UNIVERSAL | BER_PRIMITIVE)
#define BER_OBJECT     (6 | BER_CLASS_UNIVERSAL | BER_PRIMITIVE)
#define BER_SEQUENCE  (10 | BER_CLASS_UNIVERSAL | BER_CONSTRUCTED)

typedef struct {
  uint8_t* data;
  size_t len;
} oid;


#endif
