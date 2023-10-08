#include <QFlags>
#include <QDebug>

#include "backend.h"

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

void Backend::update(int state) {
	m_state = State(state);
	Q_EMIT stateChanged();
	qDebug() << "hey " << m_state;
}
