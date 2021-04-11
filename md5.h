/* MD5.H - header file for MD5C.C
 */
// MD5 context.
typedef struct {
	UINT4 state[4];             // state (ABCD) 
	UINT4 count[2];				// number of bits, modulo 2^64 (lsb first) 
	unsigned char buffer[64];   // input buffer 
} MD5_CTX;

static void MD5Init(MD5_CTX *);
static void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
static void MD5Final(unsigned char [16], MD5_CTX *);
int MDFile (char *, char *);