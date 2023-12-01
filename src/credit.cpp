#include <stdint.h>
#include <varint.h>
#include <QByteArray>

#include "credit.h"
#include "export.h"


Credit::Credit(const QString &name, const QString &description)
	: m_name(name), m_description(description) {
}

Credit::Credit(Import *im) {
	this->deserialize(im);
}

QString Credit::name() const {
	return m_name;
}

QString Credit::description() const {
	return m_description;
}

int Credit::serialize(Export *ex) {
	char *s;
	QByteArray b;

	b = m_name.toUtf8();
	s = b.data();
	ex->addItem(s, b.size());

	b = m_description.toUtf8();
	s = b.data();
	ex->addItem(s, b.size());

	return 0;
}

int Credit::deserialize(Import *im) {
	int r;
	char buf[1024]; // TODO: settable limit
	
	r = im->read(buf, 1024);
	if (!r) {
		return 1;
	}
	m_name = QString::fromUtf8(buf, r);

	r = im->read(buf, 1024);	
	if (!r) {
		return 1;
	}
	m_description = QString::fromUtf8(buf, r);
	return 0;
}	


CreditListModel::CreditListModel(QObject *parent) : QAbstractListModel(parent) {
}

int CreditListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return m_credits.count();
}

QVariant CreditListModel::data(const QModelIndex &index, int role) const {
	if (index.row() < 0 || index.row() >= m_credits.count()) {
		return QVariant();
	}
	const Credit &credit = m_credits[index.row()];
	if (role == NameRole) {
		return credit.name();
	}
	if (role == DescriptionRole) {
		return credit.description();
	}
	return QVariant();
}

void CreditListModel::addItem(const Credit &item) {
	beginInsertRows(QModelIndex(),  rowCount(), rowCount());
	m_credits << item;
	endInsertRows();
}

QHash<int, QByteArray> CreditListModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[NameRole] = "name";
	roles[DescriptionRole] = "description";
	return roles;
}
