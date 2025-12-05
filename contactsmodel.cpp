#include "contactsmodel.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDir>

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_networkManager(new QNetworkAccessManager(this))
    , m_isLoading(false)
{
    if (!initDatabase()) {
        setLastError("Failed to initialize database");
        qCritical() << "Database initialization failed";
    } else {
        loadFromDatabase();
    }
}

ContactsModel::~ContactsModel()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool ContactsModel::initDatabase()
{
    // Создаем путь к базе данных в домашней директории пользователя
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString dbPath = dataPath + "/contacts.db";
    qDebug() << "Database path:" << dbPath;
    
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        qCritical() << "Cannot open database:" << m_database.lastError().text();
        return false;
    }
    
    // Создаем таблицу, если её нет
    QSqlQuery query;
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            email TEXT NOT NULL
        )
    )";
    
    if (!query.exec(createTable)) {
        qCritical() << "Failed to create table:" << query.lastError().text();
        return false;
    }
    
    // Проверяем, есть ли данные. Если нет - добавляем начальные
    query.exec("SELECT COUNT(*) FROM contacts");
    if (query.next() && query.value(0).toInt() == 0) {
        qDebug() << "Initializing database with sample data";
        query.exec("INSERT INTO contacts (name, email) VALUES ('Bill NG', 'bill@mail.com')");
        query.exec("INSERT INTO contacts (name, email) VALUES ('John NG', 'john@mail.com')");
        query.exec("INSERT INTO contacts (name, email) VALUES ('Sam NG', 'sam@mail.com')");
    }
    
    return true;
}

bool ContactsModel::loadFromDatabase()
{
    if (!m_database.isOpen()) {
        return false;
    }
    
    beginResetModel();
    m_items.clear();
    
    QSqlQuery query("SELECT id, name, email FROM contacts ORDER BY name");
    while (query.next()) {
        Contact contact;
        contact.id = query.value(0).toInt();
        contact.name = query.value(1).toString();
        contact.email = query.value(2).toString();
        m_items.push_back(contact);
    }
    
    applyFilter();
    endResetModel();
    emit countChanged();
    
    qDebug() << "Loaded" << m_items.size() << "contacts from database";
    return true;
}

bool ContactsModel::saveToDatabase(const Contact &contact)
{
    if (!m_database.isOpen()) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (name, email) VALUES (:name, :email)");
    query.bindValue(":name", contact.name);
    query.bindValue(":email", contact.email);
    
    if (!query.exec()) {
        qCritical() << "Failed to insert contact:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ContactsModel::updateInDatabase(const Contact &contact)
{
    if (!m_database.isOpen()) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE contacts SET name = :name, email = :email WHERE id = :id");
    query.bindValue(":name", contact.name);
    query.bindValue(":email", contact.email);
    query.bindValue(":id", contact.id);
    
    if (!query.exec()) {
        qCritical() << "Failed to update contact:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ContactsModel::deleteFromDatabase(int id)
{
    if (!m_database.isOpen()) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete contact:" << query.lastError().text();
        return false;
    }
    
    return true;
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_filterString.isEmpty() ? m_items.size() : m_filteredItems.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }
    
    const Contact &contact = m_filterString.isEmpty() 
        ? m_items[index.row()] 
        : m_filteredItems[index.row()];
    
    switch (role) {
    case NameRole:
        return contact.name;
    case EmailRole:
        return contact.email;
    case IdRole:
        return contact.id;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[EmailRole] = "email";
    roles[IdRole] = "contactId";
    return roles;
}

bool ContactsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount() || count < 0) {
        return false;
    }
    
    beginInsertRows(parent, row, row + count - 1);
    
    for (int i = 0; i < count; i++) {
        Contact contact;
        contact.id = -1; // Временный ID, будет назначен при сохранении
        contact.name = "";
        contact.email = "";
        
        // Вставляем в вектор
        m_items.insert(m_items.begin() + row + i, contact);
    }
    
    endInsertRows();
    emit countChanged();
    return true;
}

void ContactsModel::appendEmptyRow()
{
    insertRows(rowCount(), 1, QModelIndex());
}

bool ContactsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return false;
    }
    
    // Получаем ссылку на контакт
    Contact *contact = nullptr;
    if (m_filterString.isEmpty()) {
        contact = &m_items[index.row()];
    } else {
        // Находим оригинальный контакт в m_items
        int contactId = m_filteredItems[index.row()].id;
        for (auto &c : m_items) {
            if (c.id == contactId) {
                contact = &c;
                break;
            }
        }
    }
    
    if (!contact) {
        return false;
    }
    
    QString newValue = value.toString();
    
    // Валидация
    if (role == EmailRole && !newValue.isEmpty()) {
        if (!isValidEmail(newValue)) {
            emit validationError("Invalid email format");
            return false;
        }
    }
    
    if (role == NameRole && !newValue.isEmpty()) {
        if (!isValidName(newValue)) {
            emit validationError("Name must be at least 2 characters");
            return false;
        }
    }
    
    // Применяем изменение
    switch (role) {
    case NameRole:
        contact->name = newValue;
        break;
    case EmailRole:
        contact->email = newValue;
        break;
    default:
        return false;
    }
    
    // Сохраняем в БД
    if (contact->id == -1) {
        // Новый контакт - проверяем, заполнены ли оба поля
        if (!contact->name.isEmpty() && !contact->email.isEmpty()) {
            if (saveToDatabase(*contact)) {
                // Получаем назначенный ID
                QSqlQuery query("SELECT last_insert_rowid()");
                if (query.next()) {
                    contact->id = query.value(0).toInt();
                }
            }
        }
    } else {
        // Обновляем существующий контакт
        updateInDatabase(*contact);
    }
    
    emit dataChanged(index, index, {role});
    applyFilter(); // Обновляем фильтр если активен
    
    return true;
}

bool ContactsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > rowCount()) {
        return false;
    }
    
    beginRemoveRows(parent, row, row + count - 1);
    
    for (int i = 0; i < count; i++) {
        const Contact &contact = m_filterString.isEmpty() 
            ? m_items[row] 
            : m_filteredItems[row];
        
        // Удаляем из БД
        if (contact.id != -1) {
            deleteFromDatabase(contact.id);
        }
        
        // Удаляем из векторов
        if (m_filterString.isEmpty()) {
            m_items.erase(m_items.begin() + row);
        } else {
            // Находим и удаляем из основного вектора
            int contactId = contact.id;
            auto it = std::find_if(m_items.begin(), m_items.end(), 
                [contactId](const Contact &c) { return c.id == contactId; });
            if (it != m_items.end()) {
                m_items.erase(it);
            }
            m_filteredItems.erase(m_filteredItems.begin() + row);
        }
    }
    
    endRemoveRows();
    emit countChanged();
    return true;
}

void ContactsModel::removeAt(int row)
{
    removeRows(row, 1, QModelIndex());
}

// Валидация
bool ContactsModel::isValidEmail(const QString &email) const
{
    if (email.isEmpty()) {
        return false;
    }
    
    // RFC 5322 упрощенная версия
    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex.match(email).hasMatch();
}

bool ContactsModel::isValidName(const QString &name) const
{
    // Имя должно быть не короче 2 символов и не содержать только пробелы
    return name.trimmed().length() >= 2;
}

// Поиск и фильтрация
void ContactsModel::setFilterString(const QString &filter)
{
    if (m_filterString == filter) {
        return;
    }
    
    m_filterString = filter;
    emit filterStringChanged();
    
    applyFilter();
}

void ContactsModel::applyFilter()
{
    beginResetModel();
    
    if (m_filterString.isEmpty()) {
        m_filteredItems.clear();
    } else {
        m_filteredItems.clear();
        QString filterLower = m_filterString.toLower();
        
        for (const Contact &contact : m_items) {
            if (contact.name.toLower().contains(filterLower) ||
                contact.email.toLower().contains(filterLower)) {
                m_filteredItems.push_back(contact);
            }
        }
    }
    
    endResetModel();
    emit countChanged();
}

// JSON экспорт
bool ContactsModel::exportToJson(const QString &filePath)
{
    QJsonArray contactsArray;
    
    for (const Contact &contact : m_items) {
        QJsonObject contactObj;
        contactObj["id"] = contact.id;
        contactObj["name"] = contact.name;
        contactObj["email"] = contact.email;
        contactsArray.append(contactObj);
    }
    
    QJsonDocument doc(contactsArray);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        setLastError("Cannot open file for writing: " + filePath);
        emit exportCompleted(false);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "Exported" << m_items.size() << "contacts to" << filePath;
    emit exportCompleted(true);
    return true;
}

// JSON импорт из API
void ContactsModel::importFromJsonApi(const QString &apiUrl)
{
    setIsLoading(true);
    
    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        setIsLoading(false);
        
        if (reply->error() != QNetworkReply::NoError) {
            setLastError("Network error: " + reply->errorString());
            qWarning() << "Import failed:" << reply->errorString();
            emit importCompleted(0);
            reply->deleteLater();
            return;
        }
        
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isArray()) {
            setLastError("Invalid JSON format: expected array");
            emit importCompleted(0);
            reply->deleteLater();
            return;
        }
        
        QJsonArray contactsArray = doc.array();
        int importedCount = 0;
        
        // Начинаем транзакцию для быстрой вставки
        m_database.transaction();
        
        for (const QJsonValue &value : contactsArray) {
            QJsonObject obj = value.toObject();
            
            QString name = obj["name"].toString();
            QString email = obj["email"].toString();
            
            // Валидация
            if (!isValidName(name) || !isValidEmail(email)) {
                qWarning() << "Skipping invalid contact:" << name << email;
                continue;
            }
            
            Contact contact;
            contact.id = -1;
            contact.name = name;
            contact.email = email;
            
            if (saveToDatabase(contact)) {
                importedCount++;
            }
        }
        
        m_database.commit();
        
        // Перезагружаем данные из БД
        loadFromDatabase();
        
        qDebug() << "Imported" << importedCount << "contacts from API";
        emit importCompleted(importedCount);
        
        reply->deleteLater();
    });
}

// Вспомогательные методы
void ContactsModel::setIsLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void ContactsModel::setLastError(const QString &error)
{
    if (m_lastError != error) {
        m_lastError = error;
        emit lastErrorChanged();
    }
}
