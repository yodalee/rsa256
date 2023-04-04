package RSA_pkg;

parameter MOD_WIDTH = 256;
parameter INT_WIDTH = 32;

typedef logic [MOD_WIDTH-1:0] KeyType;

typedef logic [INT_WIDTH-1:0] IntType;

typedef struct packed {
  IntType power;
  KeyType modulus;
} TwoPowerIn;
typedef KeyType TwoPowerOut;

typedef struct packed {
  KeyType a;
  KeyType b;
  KeyType modulus;
} MontgomeryIn;
typedef KeyType MontgomeryOut;

typedef struct packed {
  KeyType base;
  KeyType msg;
  KeyType key;
  KeyType modulus;
} RSAMontModIn;
typedef KeyType RSAMontModOut;

typedef struct packed {
  KeyType msg;
  KeyType key;
  KeyType modulus;
} RSAModIn;
typedef KeyType RSAModOut;


endpackage