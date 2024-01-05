#include <stdint.h>
#include <varint.h>
#include <QByteArray>

#include "credit.h"
#include "export.h"


Credit::Credit(Import *im) {
	this->deserialize(im);
}

Credit::Credit(std::string name, std::string description) {
	m_name.replace(0, name.length(), name);
	m_description.replace(0, description.length(), description);
}

QString Credit::name() const {
	return QString::fromStdString(m_name);
}

QString Credit::description() const {
	return QString::fromStdString(m_description);
}

int Credit::serialize(Export *ex) {
	char *s;
	QByteArray b;

	//b = m_name.toUtf8();
	//s = b.data();
	ex->addItem((char*)m_name.c_str(), m_name.length());

	//b = m_description.toUtf8();
	//s = b.data();
	ex->addItem((char*)m_description.c_str(), m_description.length());

	return 0;
}

int Credit::deserialize(Import *im) {
	int r;
	std::string s;
	char buf[1024]; // TODO: settable limit
	
	r = im->read(buf, 1024);
	if (!r) {
		return 1;
	}
	buf[r] = 0x0;
	//m_name = QString::fromUtf8(buf, r);
	m_name.replace(0, r, buf);

	r = im->read(buf, 1024);	
	if (!r) {
		return 1;
	}
	buf[r] = 0x0;
	//m_description = QString::fromUtf8(buf, r);
	m_description.replace(0, r, buf);
	return 0;
}	

std::ostream& operator << (std::ostream &out, const Credit *o) {
	out << std::string("name: ") << o->name().toStdString();
	// out << std::string("description: ") << o->description().toStdString(); // including this makes the string print empty in client
	return out;
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

void CreditListModel::addItem(const Credit item) {
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
