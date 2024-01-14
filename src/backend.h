#pragma once

#include <QObject>
#include <QTimer>
#include <QList>

#include "gpg.h"
#include "settings.h"
#include "db.h"
#include "credit.h"


/**
 *
 * \brief Defines individual high-level state flags for the application, for use with signal handling.
 *
 */
enum State {
	/// Identity public key exists
	KeyExist = 0x01,
	/// Identity public key is availabie in the application
	KeyLoaded = 0x02,
	/// Identity pubiic key access has been locked
	KeyLocked = 0x04,
};

/**
 *
 * \brief User interface proxy to components and data models.
 *
 * An instance is not usable unless initialized with \c init(...).
 *
 * \todo singleton?
 *
 */
class Backend : public QObject {
	Q_OBJECT
	Q_PROPERTY(unsigned int state READ state WRITE update NOTIFY stateChanged)
	Q_PROPERTY(QString fingerprint READ fingerprint NOTIFY stateChanged)

	public:
		/// Calling constructor is \e not sufficient to use the backend. \c init(...) \e must be called before backend is used in application.
		explicit Backend(QObject *parent = nullptr);
		/// Return the current state of the application.
		unsigned int state();
		/// Return the identity fingerprint for display.
		QString fingerprint();
		/**
		 *
		 * \brief Initialize with the given settings. 
		 *
		 * \param settings settings to initialize with
		 * \return 0 if successfully initialized
		 *
		 */
		int init(Settings *settings);
		/**
		 *
		 * \brief Update the current state of the application.
		 *
		 * \param set_state switch corresponding state bits on
		 * \param reset_state switch corresponding state bits off
		 *
		 */
		void update_r(State set_state, State reset_state);
		void set_credit_list(CreditListModel *model);
		/**
		 * \brief Add a browseable item to the display model.
		 *
		 * The method does \e not add the item to persistent storage.
		 * 
		 * \param action Type of item to add. The type determines how to cast the \c item pointer
		 * \param item Pointer to data of the type specified in \c action
		 * \return 0 if successfully added to the model
		 *
		 * \todo action is not in use, should be replaced with enum
		 *
		 */
		int add(int action, void* item);
		/// Emitted when application state changes.
		Q_SIGNAL void stateChanged();
		/// Emitted when identity public key is locked.
		Q_SIGNAL void keyLock();
		Q_DECLARE_FLAGS(States, State)
		Q_FLAG(States)

	public slots:
		/**
		 *
		 * UI proxy for state update.
		 *
		 * \sa update_r
		 *
		 */
		void update(int set_state);
		/**
		 *
		 * Unlock identity key with passphrase.
		 *
		 * When called, the \c State.KeyLoaded state will be set.
		 *
		 * If the backend has been initialized with settings where \c SettingsType.SETTINGS_LOCKTIME has a value greater than 0, a timer will be started to \c lock() the key after the given time (in milliseconds).
		 * 
		 * \param passphrase passphrase to decrypt key with
		 * \return 0 if successfully unlocked
		 *
		 */
		int unlock(const QString passphrase);
		/**
		 *
		 * Lock identity key.
		 *
		 * When called, the \c State.KeyLoaded state will be reset, and the \c keyLock signal will be emitted.
		 * 
		 * \param passphrase passphrase to decrypt key with
		 * \return 0 if successfully unlocked
		 *
		 */
		int lock();

		void image_catch(QString img_url);

	private:
		/// Settings used for initialization.
		Settings *m_settings;
		/// Keystore containing the identity public key.
		GpgStore m_gpg;
		/// The current state of the application.
		State m_state;
		/// Whether \c init() has been successfully called or not.
		bool m_init;
		/// The model containing all browseable items.
		CreditListModel *m_credit_model;
		/// If applicable, the timer triggering locking of the identity public key after \c SettingsType.SETTINGS_LOCKTIME.
		QTimer *m_timer_lock;
		/// Persistent storage of browseable items.
		Db *m_db;
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
