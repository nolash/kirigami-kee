#include <QDebug>
#include <QStandardPaths>
#include <sys/stat.h>

#include "settings.h"
#include "string.h"
#include "errno.h"
#include "debug.h"

Settings::Settings() {
	QString v = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	m_data = v.toStdString();
}

int Settings::init() {
	int r;
	
	r = mkdir(m_data.c_str(), S_IRUSR | S_IWUSR);
	if (r) {
		if (errno != EEXIST) {
			debugLog(DEBUG_ERROR, strerror(errno));
			return r;
		}
	}

	return 0;
}

std::string Settings::get(SettingsType) {
	return m_data;
}
