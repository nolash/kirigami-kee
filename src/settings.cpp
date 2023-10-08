#include <QDebug>
#include <QStandardPaths>

#include "settings.h"

Settings::Settings() {
	QString v = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	m_data = v.toStdString();
}

std::string Settings::get(SettingsType) {
	return std::string("foo");
}
