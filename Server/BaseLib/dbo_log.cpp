#define WINDOWS
#include "stdafx.h"
#ifndef WINDOWS
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#else
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cwctype>
#include <ctime>
#include <windows.h>
#endif


#include "dbo_log.h"

/* TODO: support console and log files */
/* TODO: use boost::log maybe */

/* default domain string */
static const char *dbo_log_domain = "dbo";

/* TODO: make these configurable */
//static int dbo_log_level = DBO_LOG_LEVEL_DEBUG;
static int dbo_log_level = DBO_LOG_LEVEL_INFO;
#define DBO_LOG_STREAM stderr
#define DBO_LOG_TIMEFORMAT "%Y-%m-%d %H:%M:%S"

void
dbo_log_domain_set(const char *domain)
{
	dbo_log_domain = domain;
}

void
dbo_log(int level, const char *fmt, ...)
{ 
	if (level <= dbo_log_level)
	{
		va_list va;
		char ts[64];
		time_t t;

		t = time(NULL);
		strftime(ts, sizeof(ts), DBO_LOG_TIMEFORMAT, localtime(&t));

		va_start(va, fmt);
		fprintf(DBO_LOG_STREAM, "[%s] [%s] ", ts, dbo_log_domain);
		vfprintf(DBO_LOG_STREAM, fmt, va);
		va_end(va);
	}
}


void
dbo_log_hex_bytes(int level, const char *buf, unsigned int buf_size, const char *fmt, ...)
{
	if (level <= dbo_log_level)
	{
		va_list va;
		char ts[64];
		time_t t;

		t = time(NULL);
		strftime(ts, sizeof(ts), DBO_LOG_TIMEFORMAT, localtime(&t));

		va_start(va, fmt);
		fprintf(DBO_LOG_STREAM, "[%s] [%s] ", ts, dbo_log_domain);
		vfprintf(DBO_LOG_STREAM, fmt, va);
		va_end(va);

		fprintf(DBO_LOG_STREAM, " %p (%d):", buf, buf_size);

		for (unsigned int n = 0; n < buf_size; n++)
		{
			//fprintf(DBO_LOG_STREAM, " 0x%02x", (unsigned char) buf[n]);
			fprintf(DBO_LOG_STREAM, " %02x", (unsigned char) buf[n]);
		}
		fprintf(DBO_LOG_STREAM, "\n");
	}
}

