#include <stdio.h>
#include "utf16.h"
#define YYCTYPE unsigned short
bool scan(const YYCTYPE * start, const YYCTYPE * const limit)
{
	__attribute__((unused)) const YYCTYPE * YYMARKER; // silence compiler warnings when YYMARKER is not used
#	define YYCURSOR start
Zs:
	/*!re2c
		re2c:yyfill:enable = 0;
		Zs = [\x20-\x20\xa0-\xa0\u1680-\u1680\u180e-\u180e\u2000-\u200a\u202f-\u202f\u205f-\u205f\u3000-\u3000];
		Zs { goto Zs; }
		* { return YYCURSOR == limit; }
	*/
}
static const unsigned int chars_Zs [] = {0x20,0x20,  0xa0,0xa0,  0x1680,0x1680,  0x180e,0x180e,  0x2000,0x200a,  0x202f,0x202f,  0x205f,0x205f,  0x3000,0x3000,  0x0,0x0};
static unsigned int encode_utf16 (const unsigned int * ranges, unsigned int ranges_count, unsigned short * s)
{
	unsigned short * const s_start = s;
	for (unsigned int i = 0; i < ranges_count; i += 2)
		for (unsigned int j = ranges[i]; j <= ranges[i + 1]; ++j)
		{
			if (j <= re2c::utf16::MAX_1WORD_RUNE)
				*s++ = j;
			else
			{
				*s++ = re2c::utf16::lead_surr(j);
				*s++ = re2c::utf16::trail_surr(j);
			}
		}
	return s - s_start;
}

int main ()
{
	YYCTYPE * buffer_Zs = new YYCTYPE [38];
	unsigned int buffer_len = encode_utf16 (chars_Zs, sizeof (chars_Zs) / sizeof (unsigned int), buffer_Zs);
	if (!scan (reinterpret_cast<const YYCTYPE *> (buffer_Zs), reinterpret_cast<const YYCTYPE *> (buffer_Zs + buffer_len)))
		printf("test 'Zs' failed\n");
	delete [] buffer_Zs;
	return 0;
}
