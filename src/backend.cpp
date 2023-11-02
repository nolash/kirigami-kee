#include <QFlags>
#include <QDebug>

#include "backend.h"
#include "gpg.h"
#include "settings.h"


Backend::Backend(QObject *parent) : QObject(parent) {
	m_state = State(0);
}

//State Backend::state() {
unsigned int Backend::state() {
	if (!m_init) {
		update(42);
	}
	return m_state;
}

int Backend::init(Settings *settings) {
	m_settings = settings;
	return 0;
}

int Backend::unlock(const QString passphrase) {
	int r;
	r = m_gpg.check(m_settings->get(SETTINGS_DATA), passphrase.toStdString());
	return r;
}

void Backend::update(int state) {
	m_state = State(state);
	Q_EMIT stateChanged();
	qDebug() << "hey " << m_state;
}
