# Notebook CRUD - Приложение для управления контактами

Простое Qt Quick приложение для управления списком контактов с возможностью добавления, редактирования и удаления записей.

## Скриншоты и функции

- ✅ Просмотр списка контактов
- ✅ Добавление новых контактов
- ✅ Редактирование имени и email (клик по тексту)
- ✅ Удаление контактов (свайп влево)

## Требования

### Windows

- **Qt 5.15.x** (рекомендуется 5.15.17)
- **MSYS2** с MinGW64 toolchain
- **CMake** 3.16 или выше
- **Компилятор**: GCC (из MSYS2)

### Установка зависимостей на Windows

#### 1. Установка MSYS2

1. Скачайте MSYS2 с [официального сайта](https://www.msys2.org/)
2. Установите в `C:\msys64`
3. Откройте MSYS2 MinGW 64-bit терминал
4. Обновите систему:
```bash
pacman -Syu
```
5. Установите Qt и инструменты сборки:
```bash
pacman -S mingw-w64-x86_64-qt5
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-gcc
```

#### 2. Установка Qt Creator (опционально)

Скачайте и установите Qt Creator с [официального сайта Qt](https://www.qt.io/download-qt-installer)

При установке выберите:
- Qt 5.15.x
- MinGW 8.1.0 64-bit (или используйте MSYS2)
- Qt Quick Controls 2

## Сборка проекта

### Вариант 1: Qt Creator (Рекомендуется)

1. Откройте `CMakeLists.txt` в Qt Creator
2. Выберите kit: **Desktop Qt 5.15.x MinGW 64-bit**
3. Нажмите **Configure Project**
4. Нажмите **Build** (Ctrl+B)
5. Нажмите **Run** (Ctrl+R)

### Вариант 2: Командная строка (MSYS2)

```bash
# Перейдите в папку проекта
cd Notebook_CRUD

# Создайте папку для сборки
mkdir build
cd build

# Настройте CMake
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/msys64/mingw64 ..

# Соберите проект
ninja

# Запустите приложение
./Notebook_CRUD.exe
```

### Вариант 3: CLion

1. Откройте проект в CLion
2. Перейдите в **File → Settings → Build, Execution, Deployment → CMake**
3. Добавьте в **CMake options**:
   ```
   -DCMAKE_PREFIX_PATH=C:/msys64/mingw64
   ```
4. Добавьте в **Environment**:
   ```
   PATH=C:\msys64\mingw64\bin;%PATH%
   ```
5. Нажмите **Apply → OK**
6. Нажмите **Tools → CMake → Reload CMake Project**
7. Соберите и запустите проект

## Структура проекта

```
Notebook_CRUD/
├── CMakeLists.txt          # Конфигурация сборки
├── CMakePresets.json       # Пресеты CMake для разных IDE
├── main.cpp                # Точка входа приложения
├── contactsmodel.h         # Заголовок модели данных
├── contactsmodel.cpp       # Реализация модели данных
├── main.qml                # Главное окно приложения
├── ListExample.qml         # Компонент списка контактов
├── EditableText.qml        # Компонент редактируемого текста
├── qml.qrc                 # Файл ресурсов QML
└── README.md               # Этот файл
```

## Архитектура

- **C++ Backend**: `ContactsModel` - наследник `QAbstractListModel` для управления данными
- **QML Frontend**: Интерфейс на Qt Quick Controls 2
- **Паттерн**: Model-View (Qt Model/View)

### Основные компоненты

#### ContactsModel (C++)
- Хранит список контактов в `std::vector`
- Предоставляет роли: `name`, `email`
- Методы: `appendEmptyRow()`, `removeAt()`, `setData()`

#### main.qml
- Главное окно `ApplicationWindow`
- Панель инструментов с кнопкой добавления
- Контейнер для списка контактов

#### ListExample.qml
- `ListView` для отображения контактов
- `SwipeDelegate` для каждого элемента
- Поддержка свайпа для удаления

#### EditableText.qml
- Переключение между режимами просмотра и редактирования
- Сохранение изменений при потере фокуса

## Решение проблем

### Ошибка: "Could not find Qt"

Убедитесь, что переменная `CMAKE_PREFIX_PATH` указывает на установку Qt:
```bash
-DCMAKE_PREFIX_PATH=C:/msys64/mingw64
```

### Ошибка: "g++.exe is not able to compile"

Добавьте путь к MSYS2 в переменную `PATH`:
```
PATH=C:\msys64\mingw64\bin;%PATH%
```

### Ошибка: "Cannot find DLL files" при запуске

Скопируйте необходимые DLL из `C:\msys64\mingw64\bin\` в папку с .exe или добавьте путь в PATH.

## Разработка

### Добавление новых полей в контакт

1. Добавьте поле в структуру `Contact` в `contactsmodel.h`
2. Добавьте новую роль в enum
3. Обновите методы `data()`, `setData()` и `roleNames()`
4. Добавьте компонент `EditableText` в `ListExample.qml`

### Сохранение данных

Текущая версия хранит данные в памяти. Для сохранения на диск можно добавить:
- Сериализацию в JSON (используя `QJsonDocument`)
- Сохранение в SQLite (используя `QSqlDatabase`)
- Настройки приложения через `QSettings`

## Лицензия

MIT License

## Авторы

- Разработано для изучения Qt Quick и Qt Model/View Framework

## Контакты

При возникновении проблем создайте issue в репозитории проекта.

