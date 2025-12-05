#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QStandardPaths>
#include "../contactsmodel.h"

class TestContactsModel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Тесты базовых операций
    void testInitialData();
    void testRowCount();
    void testDataRetrieval();
    void testRoleNames();
    
    // Тесты CRUD операций
    void testAppendEmptyRow();
    void testInsertRows();
    void testSetData();
    void testRemoveRows();
    void testRemoveAt();
    
    // Тесты валидации
    void testEmailValidation();
    void testEmailValidation_data();
    void testNameValidation();
    void testNameValidation_data();
    void testSetDataWithInvalidEmail();
    
    // Тесты поиска и фильтрации
    void testFilterString();
    void testFilterByName();
    void testFilterByEmail();
    void testClearFilter();
    
    // Тесты JSON экспорта
    void testExportToJson();
    void testExportToJsonEmptyList();
    
    // Тесты свойств
    void testIsLoadingProperty();
    void testCountProperty();

private:
    ContactsModel *model;
    QTemporaryDir *tempDir;
};

void TestContactsModel::initTestCase()
{
    // Используем временную директорию для тестовой БД
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
    
    QStandardPaths::setTestModeEnabled(true);
}

void TestContactsModel::cleanupTestCase()
{
    delete tempDir;
}

void TestContactsModel::init()
{
    // Создаем новую модель перед каждым тестом
    model = new ContactsModel();
    QVERIFY(model != nullptr);
}

void TestContactsModel::cleanup()
{
    delete model;
    model = nullptr;
}

// ========== Тесты базовых операций ==========

void TestContactsModel::testInitialData()
{
    // При инициализации должны быть начальные данные
    QVERIFY(model->rowCount() >= 3);
}

void TestContactsModel::testRowCount()
{
    int initialCount = model->rowCount();
    QVERIFY(initialCount >= 0);
    
    model->appendEmptyRow();
    QCOMPARE(model->rowCount(), initialCount + 1);
}

void TestContactsModel::testDataRetrieval()
{
    if (model->rowCount() == 0) {
        QSKIP("No data to test");
    }
    
    QModelIndex index = model->index(0, 0);
    QVERIFY(index.isValid());
    
    QVariant nameData = model->data(index, ContactsModel::NameRole);
    QVERIFY(nameData.isValid());
    QVERIFY(nameData.canConvert<QString>());
    
    QVariant emailData = model->data(index, ContactsModel::EmailRole);
    QVERIFY(emailData.isValid());
    QVERIFY(emailData.canConvert<QString>());
}

void TestContactsModel::testRoleNames()
{
    QHash<int, QByteArray> roles = model->roleNames();
    
    QVERIFY(roles.contains(ContactsModel::NameRole));
    QVERIFY(roles.contains(ContactsModel::EmailRole));
    QVERIFY(roles.contains(ContactsModel::IdRole));
    
    QCOMPARE(roles[ContactsModel::NameRole], QByteArray("name"));
    QCOMPARE(roles[ContactsModel::EmailRole], QByteArray("email"));
    QCOMPARE(roles[ContactsModel::IdRole], QByteArray("contactId"));
}

// ========== Тесты CRUD операций ==========

void TestContactsModel::testAppendEmptyRow()
{
    int initialCount = model->rowCount();
    
    QSignalSpy countSpy(model, &ContactsModel::countChanged);
    model->appendEmptyRow();
    
    QCOMPARE(model->rowCount(), initialCount + 1);
    QCOMPARE(countSpy.count(), 1);
}

void TestContactsModel::testInsertRows()
{
    int initialCount = model->rowCount();
    int insertPosition = 0;
    int insertCount = 2;
    
    bool success = model->insertRows(insertPosition, insertCount, QModelIndex());
    
    QVERIFY(success);
    QCOMPARE(model->rowCount(), initialCount + insertCount);
}

void TestContactsModel::testSetData()
{
    model->appendEmptyRow();
    int row = model->rowCount() - 1;
    QModelIndex index = model->index(row, 0);
    
    QSignalSpy dataSpy(model, &ContactsModel::dataChanged);
    
    // Устанавливаем имя
    bool nameSet = model->setData(index, "Test User", ContactsModel::NameRole);
    QVERIFY(nameSet);
    
    // Устанавливаем email
    bool emailSet = model->setData(index, "test@example.com", ContactsModel::EmailRole);
    QVERIFY(emailSet);
    
    // Проверяем что данные сохранились
    QCOMPARE(model->data(index, ContactsModel::NameRole).toString(), QString("Test User"));
    QCOMPARE(model->data(index, ContactsModel::EmailRole).toString(), QString("test@example.com"));
    
    QVERIFY(dataSpy.count() >= 2);
}

void TestContactsModel::testRemoveRows()
{
    if (model->rowCount() == 0) {
        model->appendEmptyRow();
    }
    
    int initialCount = model->rowCount();
    
    QSignalSpy countSpy(model, &ContactsModel::countChanged);
    bool success = model->removeRows(0, 1, QModelIndex());
    
    QVERIFY(success);
    QCOMPARE(model->rowCount(), initialCount - 1);
    QCOMPARE(countSpy.count(), 1);
}

void TestContactsModel::testRemoveAt()
{
    if (model->rowCount() == 0) {
        model->appendEmptyRow();
    }
    
    int initialCount = model->rowCount();
    model->removeAt(0);
    
    QCOMPARE(model->rowCount(), initialCount - 1);
}

// ========== Тесты валидации ==========

void TestContactsModel::testEmailValidation()
{
    QFETCH(QString, email);
    QFETCH(bool, expected);
    
    QCOMPARE(model->isValidEmail(email), expected);
}

void TestContactsModel::testEmailValidation_data()
{
    QTest::addColumn<QString>("email");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("valid standard") << "test@example.com" << true;
    QTest::newRow("valid subdomain") << "user@mail.example.com" << true;
    QTest::newRow("valid with dot") << "first.last@example.com" << true;
    QTest::newRow("valid with numbers") << "user123@example.com" << true;
    QTest::newRow("valid with plus") << "user+tag@example.com" << true;
    
    QTest::newRow("invalid no @") << "testexample.com" << false;
    QTest::newRow("invalid no domain") << "test@" << false;
    QTest::newRow("invalid no tld") << "test@example" << false;
    QTest::newRow("invalid spaces") << "test @example.com" << false;
    QTest::newRow("invalid empty") << "" << false;
    QTest::newRow("invalid only @") << "@" << false;
}

void TestContactsModel::testNameValidation()
{
    QFETCH(QString, name);
    QFETCH(bool, expected);
    
    QCOMPARE(model->isValidName(name), expected);
}

void TestContactsModel::testNameValidation_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("expected");
    
    QTest::newRow("valid short") << "Jo" << true;
    QTest::newRow("valid normal") << "John Doe" << true;
    QTest::newRow("valid long") << "Alexander von Humboldt" << true;
    QTest::newRow("valid unicode") << "Иван Иванов" << true;
    
    QTest::newRow("invalid empty") << "" << false;
    QTest::newRow("invalid one char") << "A" << false;
    QTest::newRow("invalid only spaces") << "   " << false;
    QTest::newRow("invalid one char with spaces") << " A " << false;
}

void TestContactsModel::testSetDataWithInvalidEmail()
{
    model->appendEmptyRow();
    int row = model->rowCount() - 1;
    QModelIndex index = model->index(row, 0);
    
    QSignalSpy validationSpy(model, &ContactsModel::validationError);
    
    bool result = model->setData(index, "invalid-email", ContactsModel::EmailRole);
    
    QVERIFY(!result);
    QCOMPARE(validationSpy.count(), 1);
}

// ========== Тесты поиска и фильтрации ==========

void TestContactsModel::testFilterString()
{
    QSignalSpy filterSpy(model, &ContactsModel::filterStringChanged);
    
    model->setFilterString("test");
    QCOMPARE(model->filterString(), QString("test"));
    QCOMPARE(filterSpy.count(), 1);
    
    // Повторная установка того же значения не должна генерировать сигнал
    model->setFilterString("test");
    QCOMPARE(filterSpy.count(), 1);
}

void TestContactsModel::testFilterByName()
{
    // Добавляем тестовые данные
    model->appendEmptyRow();
    int row = model->rowCount() - 1;
    QModelIndex index = model->index(row, 0);
    model->setData(index, "Alice Smith", ContactsModel::NameRole);
    model->setData(index, "alice@example.com", ContactsModel::EmailRole);
    
    int totalCount = model->rowCount();
    
    // Применяем фильтр
    model->setFilterString("Alice");
    
    // Должен найти хотя бы один контакт
    QVERIFY(model->rowCount() > 0);
    QVERIFY(model->rowCount() <= totalCount);
    
    // Очищаем фильтр
    model->setFilterString("");
    QCOMPARE(model->rowCount(), totalCount);
}

void TestContactsModel::testFilterByEmail()
{
    model->appendEmptyRow();
    int row = model->rowCount() - 1;
    QModelIndex index = model->index(row, 0);
    model->setData(index, "Bob Johnson", ContactsModel::NameRole);
    model->setData(index, "bob@unique-domain.com", ContactsModel::EmailRole);
    
    model->setFilterString("unique-domain");
    
    QVERIFY(model->rowCount() > 0);
    
    // Проверяем что найденный контакт содержит искомую строку
    QModelIndex firstResult = model->index(0, 0);
    QString email = model->data(firstResult, ContactsModel::EmailRole).toString();
    QVERIFY(email.contains("unique-domain", Qt::CaseInsensitive));
}

void TestContactsModel::testClearFilter()
{
    int initialCount = model->rowCount();
    
    model->setFilterString("some random text that matches nothing");
    QVERIFY(model->rowCount() <= initialCount);
    
    model->setFilterString("");
    QCOMPARE(model->rowCount(), initialCount);
}

// ========== Тесты JSON экспорта ==========

void TestContactsModel::testExportToJson()
{
    QString filePath = tempDir->filePath("contacts.json");
    
    QSignalSpy exportSpy(model, &ContactsModel::exportCompleted);
    bool result = model->exportToJson(filePath);
    
    QVERIFY(result);
    QCOMPARE(exportSpy.count(), 1);
    
    // Проверяем что файл создан
    QVERIFY(QFile::exists(filePath));
    
    // Проверяем что JSON валидный
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVERIFY(doc.isArray());
    
    QJsonArray array = doc.array();
    QCOMPARE(array.size(), model->rowCount());
}

void TestContactsModel::testExportToJsonEmptyList()
{
    // Удаляем все контакты
    while (model->rowCount() > 0) {
        model->removeAt(0);
    }
    
    QString filePath = tempDir->filePath("empty_contacts.json");
    bool result = model->exportToJson(filePath);
    
    QVERIFY(result);
    QVERIFY(QFile::exists(filePath));
    
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QVERIFY(doc.isArray());
    QCOMPARE(doc.array().size(), 0);
}

// ========== Тесты свойств ==========

void TestContactsModel::testIsLoadingProperty()
{
    // Изначально не загружается
    QVERIFY(!model->isLoading());
}

void TestContactsModel::testCountProperty()
{
    QSignalSpy countSpy(model, &ContactsModel::countChanged);
    
    int initialCount = model->rowCount();
    model->appendEmptyRow();
    
    QCOMPARE(model->rowCount(), initialCount + 1);
    QVERIFY(countSpy.count() > 0);
}

// Запуск тестов
QTEST_MAIN(TestContactsModel)
#include "test_contactsmodel.moc"

