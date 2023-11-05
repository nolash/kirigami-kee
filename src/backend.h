#pragma once

#include <QObject>
#include <QTimer>
#include <QList>

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
	Q_PROPERTY(QString fingerprint READ fingerprint NOTIFY stateChanged)

	public:
		explicit Backend(QObject *parent = nullptr);
		unsigned int state();
		QString fingerprint();
		int init(Settings *settings);
		void update_r(State, State);
		Q_SIGNAL void stateChanged();
		Q_SIGNAL void keyLock();
		Q_DECLARE_FLAGS(States, State)
		Q_FLAG(States)

	public slots:
		void update(int);
		int unlock(const QString passphrase);
		int lock();

	private:
		Settings *m_settings;
		GpgStore m_gpg;
		State m_state;
		bool m_init;
		QTimer *m_timer_lock;
};

//class CreditLine : public QObject {
//	Q_OBJECT
//	Q_PROPERTY(QString name READ name WRITE setName)
//	//QML_ELEMENT
//	public:
//		QString name();
//		void setName(QString);
//
//	private:
//		QString m_name;
//		QString m_description;
//};
//
//class List : public QAbstractListModel {
//	Q_OBJECT
//	QList<CreditLine *> _data;
//
//	Q_PROPERTY(int size READ size NOTIFY sizeChanged)
//	Q_PROPERTY(QQmlListProperty<CreditLine> content READ content)
//	Q_PROPERTY(QObject * parent READ parent WRITE setParent)
//	Q_CLASSINFO("DefaultProperty", "content")
//	QML_ELEMENT
//
//	public:
//		explicit List(QObject *parent = 0) : QAbstractListModel(parent) { }
//		int rowCount(const QModelIndex &p) const override { Q_UNUSED(p) return _data.size(); }
//		int columnCount(const QModelIndex &p) const override { Q_UNUSED(p) return _data.size(); }
//		QVariant data(const QModelIndex &index, int role) const override {
//			Q_UNUSED(role)
//			return QVariant::fromValue(_data[index.row()]); }
//		int size() const { return _data.size(); }
//		QQmlListProperty<CreditLine> content() { return QQmlListProperty<CreditLine>(this, _data); }
//	signals:
//		void sizeChanged();
//};
