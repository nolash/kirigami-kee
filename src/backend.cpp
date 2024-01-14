#include <QFlags>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QString>

#include "backend.h"
#include "gpg.h"
#include "settings.h"
#include "credit.h"
#include "qr.h"
#include "command.h"


Backend::Backend(QObject *parent) : QObject(parent) {
	m_state = State(0);
	m_timer_lock = 0x0;
	m_credit_model = 0x0;
	m_busy = false;
	connect(this, &Backend::commandProcessStart, this, &Backend::parseNextCommand);
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

/**
 * \todo add duplicates check
 *
 */
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
	bool rf;
	int r;
	const unsigned char *in;
	QImage img = QImage(img_url);
	QFile *f;
	char out_encoded[1024];
	char out_decoded[1024];
	size_t out_len = 1024;
	QString cmd;

	f = new QFile(img_url);
	rf = f->remove();
	if (!rf) {
		qDebug() << "could not remove" << img_url;
	}
	if (m_busy) {
		return;
	}
	m_busy = true;
	if (img.sizeInBytes() == 0) {
		qDebug() << "zeroimg" << img_url;
		m_busy = false;
		return;
	}
	qDebug() << "haveimg" << img_url;
	img = img.convertToFormat(QImage::Format_Grayscale8);
	in = img.bits();
	r = qr_y800_decode((char*)in, img.sizeInBytes(), img.width(), img.height(), out_encoded, &out_len);
	qDebug() << "result" << r;
	if (r) {
		m_busy = false;
		return;
	}
	cmd = QString(out_encoded);
	m_cmd.enqueue(cmd);
	Q_EMIT commandProcessStart();
}

int Backend::set_preview(std::string s) {
	m_cmd_cur_preview = QString::fromStdString(s);
	return 0;
}

void Backend::parseNextCommand() {
	int r;
	char rr;
	QByteArray v;
	char out[1024*1024]; 
	size_t out_len = 1024*1024;

	if (m_cmd.isEmpty()) {
		qDebug() << "parsenext command on empty queue";
	}
	m_cmd_cur = m_cmd.dequeue();
	//qDebug() << s;

	v = m_cmd_cur.toLocal8Bit();
	//r = preview_command(v.data(), v.length(), out, &out_len);
	r = process_rpc_command(this, v.data(), v.length(), true);
	
	Q_EMIT commandProcessView(r, m_cmd_cur_preview);
}

void Backend::acceptCurrentCommand() {
	int r;
	char rr;
	QByteArray v;
	char out[1024*1024];

	if (m_cmd_cur == "") {
		qDebug() << "parse current command on empty command";
	}

	v = m_cmd_cur.toLocal8Bit();
	r = process_rpc_command(this, v.data(), v.length(), false);
	if (r) {
		qDebug() << "current command is invalid, should not happen";
		m_cmd_cur = QString("");
	}
	
	m_busy = false; // replace with state
	Q_EMIT commandProcessEnd(r);

}
