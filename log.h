/*
 * $Id$
 *
 *  captagent - Homer capture agent. Modular
 *  Duplicate SIP messages in Homer Encapulate Protocol [HEP] [ipv6 version]
 *
 *  Author: Alexandr Dubovikov <alexandr.dubovikov@gmail.com>
 *  (C) Homer Project 2012-2015 (http://www.sipcapture.org)
 *
 * Homer capture agent is free software; you can redistribute it and/or
 * modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version
 *
 * Homer capture agent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/

#ifndef LOG_H_
#define LOG_H_

#include <syslog.h>

#define PA_GCC_PRINTF_ATTR(a,b) __attribute__ ((format (printf, a, b)));

#define LEMERG(fmt, args...) syslog(LOG_EMERG, "[DEBUG] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LALERT(fmt, args...) syslog(LOG_ALERT, "[ALERT] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LCRIT(fmt, args...) syslog(LOG_CRIT, "[CRIT] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LERR(fmt, args...) syslog(LOG_ERR, "[ERR] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LWARNING(fmt, args...) syslog(LOG_WARNING, "[WARNING] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LNOTICE(fmt, args...) syslog(LOG_NOTICE, "[NOTICE] " fmt, ## args)
#define LINFO(fmt, args...) syslog(LOG_INFO, "[INFO] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LDEBUG(fmt, args...) syslog(LOG_DEBUG, "[DEBUG] %s:%d " fmt, __FILE__, __LINE__, ## args)
#define LMESSAGE(fmt, args...) syslog(LOG_ERR, "[MESSAGE] " fmt, ## args)

#endif /* LOG_H_ */
