#ifndef _DEBUG_H
#define _DEBUG_H

enum debugLevel {
	DEBUG_CRITICAL,
	DEBUG_ERROR,
	DEBUG_WARNING,
	DEBUG_INFO,
	DEBUG_DEBUG,
	DEBUG_TRACE,
};

#ifdef __cplusplus
extern "C" {
#endif

void debugLog(enum debugLevel level, const char *s);

#ifdef __cplusplus
}
#endif

#endif
