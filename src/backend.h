#pragma once

#include <QObject>

#include "gpg.h"
#include "settings.h"


enum State {
	KeyExist = 0x01,
	KeyLoaded = 0x02,
	KeyLocked = 0x04,
};

class Backend : public QObject {
	Q_OBJECT
	Q_PROPERTY(unsigned int state READ state WRITE update NOTIFY stateChanged)
	Q_PROPERTY(QString fingerprint READ fingerprint)

	public:
		explicit Backend(QObject *parent = nullptr);
		unsigned int state();
		QString fingerprint();
		int init(Settings *settings);
		void update_r(State, State);
		Q_SIGNAL void stateChanged();
		Q_SIGNAL void lock();
		Q_DECLARE_FLAGS(States, State)
		Q_FLAG(States)
		
	public slots:
		void update(int);
		int unlock(const QString passphrase);

	private:
		Settings *m_settings;
		GpgStore m_gpg;
		State m_state;
		bool m_init;
};
