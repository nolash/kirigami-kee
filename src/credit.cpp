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

	s = (char*)m_name.c_str();
	ex->addItem(s, m_name.length());

	s = (char*)m_description.c_str();
	ex->addItem(s, m_description.length());
	return 0;
}

/**
 * \todo configurable buffer size
 * 
 */
int Credit::deserialize(Import *im) {
	int r;
	std::string s;
	char buf[1024];
	
	r = im->read(buf, 1024);
	if (!r) {
		return 1;
	}
	buf[r] = 0x0;
	m_name.replace(0, r, buf);

	r = im->read(buf, 1024);	
	if (!r) {
		return 1;
	}
	buf[r] = 0x0;
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
	beginInsertRows(QModelIndex(), 0, 0);
	m_credits.prepend(item);
	endInsertRows();
}

QHash<int, QByteArray> CreditListModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[NameRole] = "name";
	roles[DescriptionRole] = "description";
	return roles;
}
