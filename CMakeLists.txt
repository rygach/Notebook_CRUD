cmake_minimum_required(VERSION 3.16)
project(Notebook_CRUD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найдём Qt (любую из версий 5 или 6). Просим Core, Quick и Qml
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Qml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Qml)

# Список исходников проекта — включаем ваши .cpp/.h и qml ресурсы
set(PROJECT_SOURCES
    main.cpp
    contactsmodel.cpp
    contactsmodel.h
    main.qml         # <-- добавьте сюда реальный путь к main.qml в исходниках
    qml.qrc

)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # Qt6: используем современную QML CMake API
    qt_add_executable(Notebook_CRUD
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    # Регистрируем QML-модуль — это нужно если вы хотите, чтобы QML_ELEMENT работал автоматически.
    qt_add_qml_module(Notebook_CRUD
        URI NotebookCRUD
        VERSION 1.0
        QML_FILES main.qml        # перечислите исходные QML-файлы (не только qrc)
        SOURCES contactsmodel.cpp contactsmodel.h
        QML_FILES EditableText.qml
        QML_FILES EditableText.qml
    )

    target_link_libraries(Notebook_CRUD
        PRIVATE Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Qml
    )

    qt_import_qml_plugins(Notebook_CRUD)
    qt_finalize_executable(Notebook_CRUD)

else()
    # Qt5: традиционный путь — простая сборка; регистрация типов делаем в main.cpp вручную (qmlRegisterType)
    if (ANDROID)
        add_library(Notebook_CRUD SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Notebook_CRUD ${PROJECT_SOURCES})
    endif()

    target_link_libraries(Notebook_CRUD
        PRIVATE Qt5::Quick Qt5::Qml
    )
endif()

# Общие свойства / установка (как у вас было)
include(GNUInstallDirs)
install(TARGETS Notebook_CRUD
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (QT_VERSION_MAJOR EQUAL 6)
  # дополнительные свойства для macOS/iOS при необходимости
endif()

# Bundle properties (если нужно)
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Notebook_CRUD)
endif()
set_target_properties(Notebook_CRUD PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
