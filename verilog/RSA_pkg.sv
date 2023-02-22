package RSA_pkg;

parameter MOD_WIDTH = 256;
parameter INT_WIDTH = 32;

typedef logic [MOD_WIDTH-1:0] KeyType;

typedef logic [INT_WIDTH-1:0] IntType;

typedef struct {
  KeyType msg;
  KeyType key;
  KeyType modulus;
} RSAModIn;
typedef KeyType RSAModOut;

typedef struct {
  IntType power;
  KeyType modulus;
} RSATwoPowerModIn;
typedef KeyType RSATwoPowerModOut;

typedef struct {
  KeyType a;
  KeyType b;
  KeyType modulus;
} RSAMontgomeryModIn;
typedef KeyType RSAMontgomeryModOut;


endpackage