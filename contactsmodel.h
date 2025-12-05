#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <vector>

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit ContactsModel(QObject *parent = nullptr);
    ~ContactsModel();

    enum {
        NameRole = Qt::UserRole,
        EmailRole,
        IdRole
    };

    struct Contact {
        int id;
        QString name;
        QString email;
    };

    // QAbstractListModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    // CRUD операции
    virtual bool insertRows(int row, int count, const QModelIndex &parent) override;
    Q_INVOKABLE void appendEmptyRow();
    
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;
    Q_INVOKABLE void removeAt(int row);

    // Валидация
    Q_INVOKABLE bool isValidEmail(const QString &email) const;
    Q_INVOKABLE bool isValidName(const QString &name) const;

    // Поиск и фильтрация
    QString filterString() const { return m_filterString; }
    void setFilterString(const QString &filter);

    // JSON экспорт/импорт
    Q_INVOKABLE bool exportToJson(const QString &filePath);
    Q_INVOKABLE void importFromJsonApi(const QString &apiUrl);

    // Свойства
    bool isLoading() const { return m_isLoading; }
    QString lastError() const { return m_lastError; }

signals:
    void isLoadingChanged();
    void filterStringChanged();
    void lastErrorChanged();
    void countChanged();
    void validationError(const QString &message);
    void importCompleted(int count);
    void exportCompleted(bool success);

private:
    // SQLite методы
    bool initDatabase();
    bool loadFromDatabase();
    bool saveToDatabase(const Contact &contact);
    bool updateInDatabase(const Contact &contact);
    bool deleteFromDatabase(int id);
    void applyFilter();
    
    // Вспомогательные методы
    void setIsLoading(bool loading);
    void setLastError(const QString &error);

    // Данные
    std::vector<Contact> m_items;          // Все контакты
    std::vector<Contact> m_filteredItems;  // Отфильтрованные контакты
    QSqlDatabase m_database;
    QNetworkAccessManager *m_networkManager;
    
    // Состояние
    bool m_isLoading;
    QString m_filterString;
    QString m_lastError;
};

#endif // CONTACTSMODEL_H
