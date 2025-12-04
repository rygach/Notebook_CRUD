#include "contactsmodel.h"

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel{parent}
{
    // Примеры контактов для демонстрации
    m_items = std::vector<Contact>({
        {QStringLiteral("Alice Johnson"), QStringLiteral("alice.johnson@example.com")},
        {QStringLiteral("Bob Smith"), QStringLiteral("bob.smith@company.com")},
        {QStringLiteral("Charlie Brown"), QStringLiteral("charlie@email.com")}
    });
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_items.size());
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }

    const Contact &contact = m_items[index.row()];
    
    switch (role) {
    case NameRole:
        return contact.name;
    case EmailRole:
        return contact.email;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[EmailRole] = "email";
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

bool ContactsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;

    Contact & item = m_items[index.row()];
    switch (role) {
    case (NameRole):
        item.name = value.toString();
        break;
    case (EmailRole):
        item.email = value.toString();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});

    return true;
}

bool ContactsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount() || row + count < 0 || row + count > rowCount()) return false;

    beginRemoveRows(parent, row, row + count -1);

    m_items.erase(m_items.begin() + row, m_items.begin() + row + count);

    endRemoveRows();
    return true;
}

void ContactsModel::removeAt(int row)
{
    removeRows(row, 1, QModelIndex());
}
