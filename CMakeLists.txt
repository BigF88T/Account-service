cmake_minimum_required(VERSION 3.10)
project(web_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(web_service
        src/main.cpp
        src/service/account_service_impl.cpp
        src/service/account_service_db_impl.cpp
        src/controller/account_controller.cpp
        include/account_service_db_impl.h
        src/service/account_service_db_impl.cpp
        src/configuration/migration_runner.cpp
        include/migration_runner.h
        include/db_util.h
        src/configuration/configuration.cpp
        include/configuration.h
        src/util/db_util.cpp
)

# Заголовки (Наши + Asio + Crow + PostgreSQL)
target_include_directories(web_service PUBLIC
        include
        external/asio/include
        external/Crow/include
        external/pgsql/include # Заголовки libpq
)

# Настройки для Crow
target_compile_definitions(web_service PRIVATE
        ASIO_STANDALONE
        CROW_CAN_USE_ASIO_STANDALONE
)

# Путь к библиотеке PostgreSQL (libpq)
# libpq.lib - это библиотека импорта для Windows
target_link_directories(web_service PRIVATE external/pgsql/lib)

# Линковка системных сокетов и базы данных
if (WIN32)
    target_link_libraries(web_service PUBLIC ws2_32 mswsock libpq)
endif ()
# Описываем нахождение конфигурационного файла и копируем их в build
configure_file(resources/application.json resources/application.json COPYONLY)
#Копируем миграции в build_command()
file(COPY resources/migrations DESTINATION resources)

# Это кастомная цель для web_service,
# чтобы каждый раз при запуске копировать файлы миграций
add_custom_target(copy_migrations ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/migrations
        ${CMAKE_BINARY_DIR}/resources/migrations
        COMMENT "Копируем миграции в директорию build"
)
# Добавляем зависимость сервису на кастомную цель "copy_migrations"
add_dependencies(web_service copy_migrations)