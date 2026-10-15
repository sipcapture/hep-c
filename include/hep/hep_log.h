/*
 * Pluggable logging (issue #3).
 */
#ifndef HEP_LOG_H_
#define HEP_LOG_H_

#include <stdarg.h>

typedef enum hep_log_level {
	HEP_LOG_EMERG = 0,
	HEP_LOG_ERR,
	HEP_LOG_WARNING,
	HEP_LOG_NOTICE,
	HEP_LOG_INFO,
	HEP_LOG_DEBUG
} hep_log_level_t;

typedef void (*hep_log_callback)(hep_log_level_t level, const char *message, void *user_data);

void hep_set_log_callback(hep_log_callback callback, void *user_data);
void hep_log(hep_log_level_t level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#define HEP_LOG_EMERG(...)   hep_log(HEP_LOG_EMERG, __VA_ARGS__)
#define HEP_LOG_ERR(...)     hep_log(HEP_LOG_ERR, __VA_ARGS__)
#define HEP_LOG_WARNING(...) hep_log(HEP_LOG_WARNING, __VA_ARGS__)
#define HEP_LOG_NOTICE(...)  hep_log(HEP_LOG_NOTICE, __VA_ARGS__)
#define HEP_LOG_INFO(...)    hep_log(HEP_LOG_INFO, __VA_ARGS__)
#define HEP_LOG_DEBUG(...)   hep_log(HEP_LOG_DEBUG, __VA_ARGS__)

#endif /* HEP_LOG_H_ */
