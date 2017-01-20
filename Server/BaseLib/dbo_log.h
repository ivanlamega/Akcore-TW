#ifndef DBO_LOG_H
#define DBO_LOG_H


/**
 * Simple logging functions.
 */

enum {
	DBO_LOG_LEVEL_NONE = 0,
	DBO_LOG_LEVEL_ERROR,
	DBO_LOG_LEVEL_WARN,
	DBO_LOG_LEVEL_INFO,
	DBO_LOG_LEVEL_DEBUG,
};

void dbo_log_domain_set(const char *domain);
void dbo_log(int level, const char *fmt, ...);
void dbo_log_hex_bytes(int level, const char *buf, unsigned int buf_size, const char *fmt, ...);


#endif

