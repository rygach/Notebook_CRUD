#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <vector>

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ContactsModel(QObject *parent = nullptr);

    enum {
        NameRole = Qt::UserRole,
        EmailRole
    };

    struct Contact {
        QString name;
        QString email;
    };

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Contact> m_items;
};

#endif // CONTACTSMODEL_H
