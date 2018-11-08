#include <stdio.h>
#include "utf8.h"
#define YYCTYPE unsigned char
bool scan(const YYCTYPE * start, const YYCTYPE * const limit)
{
	__attribute__((unused)) const YYCTYPE * YYMARKER; // silence compiler warnings when YYMARKER is not used
#	define YYCURSOR start
Cs:
	/*!re2c
		re2c:yyfill:enable = 0;
		Cs = [\ud800-\udfff];
		Cs { goto Cs; }
		* { return YYCURSOR == limit; }
	*/
}
static const unsigned int chars_Cs [] = {0xd800,0xdfff,  0x0,0x0};
static unsigned int encode_utf8 (const unsigned int * ranges, unsigned int ranges_count, unsigned char * s)
{
	unsigned char * const s_start = s;
	for (unsigned int i = 0; i < ranges_count - 2; i += 2)
		for (unsigned int j = ranges[i]; j <= ranges[i + 1]; ++j)
			s += re2c::utf8::rune_to_bytes (s, j);
	re2c::utf8::rune_to_bytes (s, ranges[ranges_count - 1]);
	return s - s_start + 1;
}

int main ()
{
	YYCTYPE * buffer_Cs = new YYCTYPE [8196];
	unsigned int buffer_len = encode_utf8 (chars_Cs, sizeof (chars_Cs) / sizeof (unsigned int), buffer_Cs);
	if (!scan (reinterpret_cast<const YYCTYPE *> (buffer_Cs), reinterpret_cast<const YYCTYPE *> (buffer_Cs + buffer_len)))
		printf("test 'Cs' failed\n");
	delete [] buffer_Cs;
	return 0;
}
