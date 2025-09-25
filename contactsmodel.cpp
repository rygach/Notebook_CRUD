#include "contactsmodel.h"
#include "QDebug"

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel{parent}
{
    m_items = std::vector<Contact>(
        {
            {QStringLiteral("Bill NG"), QStringLiteral("bill@mail.com")},
            {QStringLiteral("John NG"), QStringLiteral("john@mail.com")},
            {QStringLiteral("Sam NG"), QStringLiteral("sam@mail.com")}
        });
}

int ContactsModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    qDebug() << "rowCount() is called";
    return m_items.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant(false);

    qDebug() << "data() is called";

    switch (role) {
    case NameRole:
        return m_items[index.row()].name;
    case EmailRole:
        return m_items[index.row()].email;
    }

    return QVariant(false);
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[EmailRole] = "email";

    qDebug() << "roleNames() is called";
    return roles;
}

bool ContactsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount() || count < 0) return false;

    beginInsertRows(parent, row, row + count -1);

    for (int i = row; i < row + count; i++) {
        m_items.insert(m_items.begin() + i, Contact{});
    }

    endInsertRows();
    return true;
}

void ContactsModel::appendEmptyRow()
{
    insertRows(rowCount(), 1, QModelIndex());
}
