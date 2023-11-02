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
		update_r(State(0), State(0));
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
	if (r) {
		Q_EMIT lock();
	} else {
		update_r(State(KeyLoaded), State(0));
	}
	return r;
}

void Backend::update(int state) {
	//update_r(instate, outstate);
}

void Backend::update_r(State instate, State outstate) {
	m_state = instate;
	Q_EMIT stateChanged();
	qDebug() << "statechange " << m_state;
}

QString Backend::fingerprint() {
	char *s;
	s = m_gpg.get_fingerprint();
	return QString(s);
}
