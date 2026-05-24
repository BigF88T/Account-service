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
