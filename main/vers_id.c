#include "vers_id.h"

#ifndef DESCENT_VERSION_EXTRA
#define DESCENT_VERSION_EXTRA	"v" VERSION
#endif

const char g_descent_version[] = "D2X-Rebirth " DESCENT_VERSION_EXTRA " " __DATE__ " " __TIME__;
