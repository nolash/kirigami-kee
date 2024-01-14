#include <QFlags>
#include <QDebug>
#include <QImage>
#include <QFile>

#include "backend.h"
#include "gpg.h"
#include "settings.h"
#include "credit.h"
#include "qr.h"
#include "transport.h"


Backend::Backend(QObject *parent) : QObject(parent) {
	m_state = State(0);
	m_timer_lock = 0x0;
	m_credit_model = 0x0;
}

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

void Backend::set_credit_list(CreditListModel *credit_model) {
	m_credit_model = credit_model;
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

int Backend::add(int action, void *item) {
	Credit *credit = (Credit*)item;
	m_credit_model->addItem(*credit);
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

void Backend::update(int instate) {
	update_r(State(instate), State(0));
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

void Backend::image_catch(QString img_url) {
	int r;
	const unsigned char *in;
	QImage img = QImage(img_url);
	QFile *f;
	char out_encoded[1024];
	char out_decoded[1024];
	size_t out_len = 1024;
	
	f = new QFile(img_url);
	f->remove();
	qDebug() << "haveimg" << img_url;
	img = img.convertToFormat(QImage::Format_Grayscale8);
	in = img.bits();
	r = qr_y800_decode((char*)in, img.sizeInBytes(), img.width(), img.height(), out_encoded, &out_len);
	if (!r) {
		//r = unpack();
	}
	qDebug() << "result" << r;
}
