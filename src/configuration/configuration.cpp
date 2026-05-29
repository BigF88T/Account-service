//
// Created by thbr on 29.05.2026.
//

#include "../../include/configuration.h"

#include <fstream>

Configuration::Configuration(const std::string &config_file) {
    std::ifstream file(config_file.c_str());

    if (!file.is_open()) {
        throw std::runtime_error("Не получается открыть файл: " + config_file);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    crow::json::rvalue json = crow::json::load(buffer.str());
    if (!json) {
        throw std::runtime_error("Невалидный JSON в конфигурационном файле");
    }

    ParseAndValidate(json);
}

std::string Configuration::GetDbUrl() const {
    return db_config_.url;
}

int Configuration::GetPort() const {
    return server_config_.port;
}

std::string Configuration::GetMigrationsDir() const {
    return migrations_config_.migration_dir;
}

void Configuration::ParseAndValidate(const crow::json::rvalue &json) {
    if (!json.has("database") || !json["database"].has("url")) {
        throw std::runtime_error("Ошибка конфигурации: Неправильное значение database.url");
    }

    db_config_.url = json["database"]["url"].s();

    if (!json.has("server") || !json["server"].has("port")) {
        throw std::runtime_error("Ошибка конфигурации: Неправильное значение server.port");
    }

    server_config_.port = static_cast<int>(json["server"]["port"].i());

    if (!json.has("migrations") || !json["migrations"].has("dir")) {
        throw std::runtime_error("Ошибка конфигурации: Неправильное значение migrations.dir");
    }

    migrations_config_.migration_dir = json["migrations"]["dir"].s();
}
