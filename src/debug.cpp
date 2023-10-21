#include <QDebug>
#include "debug.h"

void debugLog(enum debugLevel level, const char *s) {
	qDebug() << s;
}
