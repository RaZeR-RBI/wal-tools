#include "image.h"

void flip_bgr(sptr_t colors)
{
	int i;
	int size = colors.size - (colors.size % 3);
	unsigned char *p = colors.ptr;
	for (i = 0; i < size; i += 3) {
		unsigned char tmp = *p;
		*p = *(p + 2);
		*(p + 2) = tmp;
		p += 3;
	}
}
