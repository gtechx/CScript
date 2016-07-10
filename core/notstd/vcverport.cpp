#include "stdafx.h"
#include "vcverport.h"
#include <assert.h>

#if defined(PLATFORM_WINDOWS)
#if _MSC_VER <= 1600

NOTSTD_API unsigned long long strtoull(const char *nptr, char **endptr, int base)
{
	unsigned long long r = 0;

	// ����Ŀǰ�����У����õ���10���ƵĲ��֣���������Ҳ��ʵ��ʮ�����ƵĲ���
	assert(base == 10);

	const char *p = nptr;
	for (; *p != '\0'; p++)
	{
		if (!::isdigit((unsigned char)*p))
			break;
		r *= 10;
		r += static_cast<unsigned long>((unsigned char)*p) - '0';
	}
	*endptr = (char*)p;

	return r;
}
#endif

#endif
