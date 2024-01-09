#include <QDebug>
#include <QStandardPaths>
#include <sys/stat.h>

#include "settings.h"
#include "string.h"
#include "errno.h"
#include "err.h"
#include "debug.h"

Settings::Settings() {
	QString v = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	m_data = v.toStdString();
	v = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
	m_run = v.toStdString();
	m_locktime = "0";
}

int Settings::init() {
	int r;
	std::string s;
	
	r = mkdir(m_data.c_str(), S_IRUSR | S_IWUSR);
	if (r) {
		if (errno != EEXIST) {
			debugLog(DEBUG_ERROR, strerror(errno));
			return r;
		}
	}
	s = "datadir: " + m_data + "\nrundir: " + m_run;
	debugLog(DEBUG_DEBUG, s.c_str());

	return ERR_OK;
}

std::string Settings::get(SettingsType type) {
	switch(type) {
		case SETTINGS_DATA:
			return m_data;
			break;
		case SETTINGS_RUN:
			return m_run;
			break;
		case SETTINGS_LOCKTIME:
			return m_locktime;
			break;
		default:
			return "";
	}
}

int Settings::set(SettingsType type, std::string data) {
	switch(type) {
		case SETTINGS_DATA:
			m_data = data;
			break;
		case SETTINGS_RUN:
			m_run = data;
			break;
		case SETTINGS_LOCKTIME:
			m_locktime = data;
			break;
		default:
			return ERR_FAIL;
	}
	return ERR_OK;
}
