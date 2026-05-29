//
// Created by thbr on 28.05.2026.
//

#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "libpq-fe.h"

class MigrationRunner {
public:
    explicit MigrationRunner(const std::string &conn_str);

    ~MigrationRunner() = default;

    void RunMigration(const std::string &migrations_dir) const;

private:
    std::string default_path_to_migration_ = "resources/migrations";

    std::string conn_str_;

    std::vector<std::filesystem::path> ReceiveMigrationFiles(const std::string &migrations_dir) const;

    static bool IsMigrationApplied(PGconn *conn, const std::filesystem::path &path);

    static void InitChangelogTable(PGconn *conn);

    static void BeginMigration(PGconn *conn, const std::filesystem::path &path);
};
