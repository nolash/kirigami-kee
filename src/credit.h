#ifndef _KEE_CREDIT_H
#define _KEE_CREDIT_H

#include <QAbstractListModel>

#include "export.h"
#include "item.h"

// based on https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quick/models/abstractitemmodel?h=5.15

/**
 * 
 * \brief A credit-item representing a counter-signed IOU with spends.
 *
 * \todo Define virtual class.
 */
class Credit : public UserItem {

public:
	/// Constructor for importing data from memory.
	Credit(Import *im);
	/// Constructor for directly setting values for instance.
	Credit(std::string name, std::string description);
	~Credit() {};
	/// Name getter
	QString name() const;
	/// Description getter.
	QString description() const;
	/// Serializer, for use with \c Export instance.
	int serialize(Export *ex);
	/// Deserializer, for use with \c Import instance.
	int deserialize(Import *im);
	/**
	 * Calculate digest over serialized instance.
	 *
	 * \todo Should use data in instance?
	 */
	int sum(const char *in, size_t in_len, char *out, size_t *out_len);
	std::string preview() override;
private:
	/// cached name
	std::string m_name;
	/// cached description
	std::string m_description;


};

/**
 *
 *  \brief Data model for displaying the list of stored credit-items.
 */
class CreditListModel : public QAbstractListModel {
	Q_OBJECT
public:
	/// Implements QAbstractListModel
	enum CreditRoles {
		NameRole = Qt::UserRole + 1,
		DescriptionRole
	};
	CreditListModel(QObject *parent = 0);
	/// Implements QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex())	const override;
	/// Implements QAbstractListModel
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	/**
	 * \brief Add a single credit item to the model.
	 * 
	 * The item will be prepended to the list; items should be added oldest first.
	 *
	 */
	void addItem(const Credit item);
protected:
	/// Implements QAbstractListModel
	QHash<int, QByteArray> roleNames() const override;
private:
	/// The credit-item list to display.
	QList<Credit> m_credits;

};


/// String representation of credit.
std::ostream& operator << (std::ostream &out, const Credit *o);

#endif // _KEE_CREDIT_H
