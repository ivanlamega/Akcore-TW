#include "stdafx.h"
bool dbo_unicode_utf16_to_utf8(char *dst, unsigned int *dst_size,const WCHAR *src, unsigned int src_size);
bool dbo_unicode_utf8_to_utf16(WCHAR *dst, unsigned int *dst_size,const char *src, unsigned int src_size);
unsigned int dbo_unicode_utf8_strlen(const char *str);
unsigned int dbo_unicode_utf16_strlen(const WCHAR *str);
unsigned int dbo_unicode_utf16_buflen(const WCHAR *str);