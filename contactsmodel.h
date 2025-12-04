#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <vector>
#include <qqml.h>

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

    // QAbstractListModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    // Insert operations
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    Q_INVOKABLE void appendEmptyRow();

    // Update operations
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // Delete operations
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    Q_INVOKABLE void removeAt(int row);


private:
    std::vector<Contact> m_items;
};

#endif // CONTACTSMODEL_H
