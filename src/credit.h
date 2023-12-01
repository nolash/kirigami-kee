#include <QAbstractListModel>

#include "export.h"

// based on https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quick/models/abstractitemmodel?h=5.15

class Credit {

public:
	Credit(Import *im);
	Credit(const QString &name, const QString &description);
	QString name() const;
	QString description() const;
	int serialize(Export *ex);
	int deserialize(Import *im);
private:
	QString m_name;
	QString m_description;

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
	void addItem(const Credit &item);
protected:
	QHash<int, QByteArray> roleNames() const override;
private:
	QList<Credit> m_credits;

};
