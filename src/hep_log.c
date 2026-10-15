#include "hep/hep_log.h"

#include <stdio.h>
#include <string.h>

static hep_log_callback log_cb;
static void *log_cb_data;

void hep_set_log_callback(hep_log_callback callback, void *user_data)
{
	log_cb = callback;
	log_cb_data = user_data;
}

static void default_log(hep_log_level_t level, const char *message)
{
	const char *tag = "HEP";
	FILE *out = stderr;

	switch (level) {
	case HEP_LOG_EMERG:
	case HEP_LOG_ERR:
		tag = "HEP ERR";
		break;
	case HEP_LOG_WARNING:
		tag = "HEP WARN";
		break;
	default:
		break;
	}

	fprintf(out, "%s: %s\n", tag, message);
}

void hep_log(hep_log_level_t level, const char *fmt, ...)
{
	char buf[2048];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (log_cb)
		log_cb(level, buf, log_cb_data);
	else
		default_log(level, buf);
}
