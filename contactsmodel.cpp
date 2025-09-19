#include "contactsmodel.h"
#include "QDebug"

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel{parent}
{
    m_items = std::vector<Contact>(
        {
            {QStingLiteral("Bill NG"), QStringLiteral("bill@mail.com")},
            {QStingLiteral("John NG"), QStringLiteral("john@mail.com")},
            {QStingLiteral("Sam NG"), QStringLiteral("sam@mail.com")}
        })
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << "rowCount() is called";
    return m_items.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{

}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[EmailRole] = "email";

    qDebug() << "roleNames() is called";
    return roles;
}
