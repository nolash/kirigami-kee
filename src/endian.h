#ifndef LASH_ENDIAN_H_
#define LASH_ENDIAN_H_

#define CONVERT_BIGENDIAN 0x00
#define CONVERT_LITTLEENDIAN 0x01

union le {
	short *s;
	int *i;
	long long *ll;
	unsigned char *c;
};

int le(int l, void *n);
int is_le();
int to_endian(char direction, int l, void *n);
void flip_endian(int l, union le *n);

#endif // LASH_ENDIAN_H_
