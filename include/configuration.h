//
// Created by thbr on 29.05.2026.
//

#pragma once
#include <string>

#include "crow/json.h"

class Configuration {
public:
    struct DatabaseConfig {
        std::string url;
    };

    struct ServerConfig {
        int port;
    };

    struct MigrationsConfig {
        std::string migration_dir;
    };

    explicit Configuration(const std::string &config_file);

    std::string GetDbUrl() const;

    int GetPort() const;

    std::string GetMigrationsDir() const;

private:
    DatabaseConfig db_config_;
    ServerConfig server_config_;
    MigrationsConfig migrations_config_;

    void ParseAndValidate(const crow::json::rvalue &json);
};
