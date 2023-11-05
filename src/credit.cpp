#include "credit.h"

Credit::Credit(const QString &name, const QString &description) 
	: m_name(name), m_description(description) {
}

QString Credit::name() const {
	return m_name;
}

QString Credit::description() const {
	return m_description;
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
