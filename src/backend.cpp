#include <QFlags>
#include <QDebug>

#include "backend.h"
#include "gpg.h"
#include "settings.h"


Backend::Backend(QObject *parent) : QObject(parent) {
	m_state = State(0);
	m_timer_lock = 0x0;
}

unsigned int Backend::state() {
	if (!m_init) {
		update_r(State(0), State(0));
	}
	return m_state;
}

int Backend::init(Settings *settings) {
	int r;
	m_settings = settings;
 	return 0;
}

int Backend::lock() {
	if (m_timer_lock != 0x0 && m_timer_lock->isActive()) {
		m_timer_lock->stop();
	}
	delete m_timer_lock;
	update_r(State(0), State(KeyLoaded));
	Q_EMIT keyLock();
	return 0;
}

int Backend::unlock(const QString passphrase) {
	int r;
	int locktime;
	std::string locktime_s;

	r = m_gpg.check(m_settings->get(SETTINGS_DATA), passphrase.toStdString());
	if (r) {
		qDebug() << "unlock fail";
		lock();
	} else {
		update_r(State(KeyLoaded), State(0));
		locktime_s = m_settings->get(SETTINGS_LOCKTIME);
		if (locktime_s != "0") {
			m_timer_lock = new QTimer(this);
			connect(m_timer_lock, &QTimer::timeout, this, &Backend::lock);
			locktime = std::stoi(locktime_s);
			m_timer_lock->start(locktime * 1000);
		}
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
