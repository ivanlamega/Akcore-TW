
#include "dbo_unicode.h"
#include "stdafx.h"




/* Implmentation from 7z lzma SDK, with some variables and types changed. */



/* LZMA SDK is written and placed in the public domain by Igor Pavlov. */



static unsigned char kUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };



bool dbo_unicode_utf16_to_utf8(char *dst, unsigned int *dst_size,

	const WCHAR *src, unsigned int src_size)

{

	unsigned int dstPos = 0, srcPos = 0;

	for (;;)

	{

		unsigned int numAdds;

		uint32_t value;

		if (srcPos == src_size)

		{

			if (dst_size) *dst_size = dstPos;

			return true;

		}

		value = src[srcPos++];

		if (value < 0x80)

		{

			if (dst)

				dst[dstPos] = (char)value;

			dstPos++;

			if (value == 0)

				break;

			else

				continue;

		}

		if (value >= 0xD800 && value < 0xE000)

		{

			uint32_t c2;

			if (value >= 0xDC00 || srcPos == src_size)

				break;

			c2 = src[srcPos++];

			if (c2 < 0xDC00 || c2 >= 0xE000)

				break;

			value = (((value - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;

		}

		for (numAdds = 1; numAdds < 5; numAdds++)

			if (value < (((uint32_t)1) << (numAdds * 5 + 6)))

				break;

		if (dst)

			dst[dstPos] = (char)(kUtf8Limits[numAdds - 1] + (value >> (6 * numAdds)));

		dstPos++;

		do

		{

			numAdds--;

			if (dst)

				dst[dstPos] = (char)(0x80 + ((value >> (6 * numAdds)) & 0x3F));

			dstPos++;

		}

		while (numAdds != 0);

	}

	if (dst_size) *dst_size = dstPos;

	return false;

}





bool dbo_unicode_utf8_to_utf16(WCHAR *dst, unsigned int *dst_size,

	const char *src, unsigned int src_size)

{

	unsigned int destPos = 0, srcPos = 0;

	for (;;)

	{

		uint8_t c;

		int numAdds;

		if (srcPos == src_size)

		{

			if (dst_size) *dst_size = destPos;

			return true;

		}

		c = (uint8_t)src[srcPos++];



		if (c < 0x80)

		{

			if (dst)

				dst[destPos] = (uint16_t)c;

			destPos++;

			if (c == 0)

				break;

			else

				continue;

		}

		if (c < 0xC0)

			break;

		for (numAdds = 1; numAdds < 5; numAdds++)

			if (c < kUtf8Limits[numAdds])

				break;

		uint32_t value = (c - kUtf8Limits[numAdds - 1]);



		do

		{

			uint8_t c2;

			if (srcPos == src_size)

				break;

			c2 = (uint8_t)src[srcPos++];

			if (c2 < 0x80 || c2 >= 0xC0)

				break;

			value <<= 6;

			value |= (c2 - 0x80);

		}

		while (--numAdds != 0);



		if (value < 0x10000)

		{

			if (dst)

				dst[destPos] = (uint16_t)value;

			destPos++;

		}

		else

		{

			value -= 0x10000;

			if (value >= 0x100000)

				break;

			if (dst)

			{

				dst[destPos + 0] = (uint16_t)(0xD800 + (value >> 10));

				dst[destPos + 1] = (uint16_t)(0xDC00 + (value & 0x3FF));

			}

			destPos += 2;

		}

	}

	if (dst_size) *dst_size = destPos;

	return false;

}





/* some simple utf8 length function found on google */

unsigned int

dbo_unicode_utf8_strlen(const char *str)

{

	int i = 0, j = 0;

	while (str[i])

	{

		if ((str[i] & 0xc0) != 0x80)

		{

			j++;

		}

		i++;

	}

	return j;

}





unsigned int

dbo_unicode_utf16_strlen(const WCHAR *str)

{

	int j = 0;

	WCHAR *wc = (WCHAR *)str;

	while (*wc)

	{

		if (*wc >= 0xd800 && *wc <= 0xdfff)

		{

			j++;

			wc++;

		}

		j++;

		wc++;

	}

	return j;

}





unsigned int

dbo_unicode_utf16_buflen(const WCHAR *str)

{

	int j = 0;

	WCHAR *wc = (WCHAR *)str;

	while (*wc)

	{

		j++;

		wc++;

	}

	/* including the null terminator */

	return (j + 1) * sizeof(WCHAR);

}