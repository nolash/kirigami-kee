#ifndef _CREDIT_H
#define _CREDIT_H

#include <QAbstractListModel>

#include "export.h"

// based on https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quick/models/abstractitemmodel?h=5.15

class Credit {

public:
	Credit(Import *im);
	Credit(std::string name, std::string description);
	QString name() const;
	QString description() const;
	int serialize(Export *ex);
	int deserialize(Import *im);
	int sum(const char *in, size_t in_len, char *out, size_t *out_len);
private:
	std::string m_name;
	std::string m_description;


};

class CreditListModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum CreditRoles {
		NameRole = Qt::UserRole + 1,
		DescriptionRole
	};
	CreditListModel(QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex())	const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	void addItem(const Credit item);
protected:
	QHash<int, QByteArray> roleNames() const override;
private:
	QList<Credit> m_credits;

};

std::ostream& operator << (std::ostream &out, const Credit *o);

#endif // _CREDIT_H
