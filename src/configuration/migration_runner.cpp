//
// Created by thbr on 28.05.2026.
//

#include "migration_runner.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "db_util.h"

MigrationRunner::MigrationRunner(const std::string &conn_str) : conn_str_(conn_str) {}

void MigrationRunner::RunMigration(const std::string &migrations_dir) const {
    const DbUtil::DbConnection db(conn_str_);

    InitChangelogTable(db.conn);

    const auto files = ReceiveMigrationFiles(migrations_dir);

    for (const auto &path: files) {
        if (!IsMigrationApplied(db.conn, path)) {
            BeginMigration(db.conn, path);
        }
    }
}

void MigrationRunner::BeginMigration(PGconn *conn, const std::filesystem::__cxx11::path &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл миграции: " << path << std::endl;
        throw std::runtime_error("Receive migration exception");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    PGresult *res_of_migration = PQexec(conn, sql.c_str());

    const DbUtil::ResPtr guard_begin_migration(res_of_migration);

    if (PQresultStatus(res_of_migration) == PGRES_COMMAND_OK) {
        const std::string path_to_new_migration = path.filename().string();
        const char *param_values_changelog[1] = {path_to_new_migration.c_str()};

        PGresult *changelog_result = PQexecParams(
            conn,
            "INSERT INTO database_changelog (version) VALUES ($1)",
            1,
            nullptr,
            param_values_changelog,
            nullptr,
            nullptr,
            0
        );

        const DbUtil::ResPtr guard_insert_changelog(changelog_result);

        if (PQresultStatus(changelog_result) != PGRES_COMMAND_OK) {
            throw std::runtime_error("Ошибка записи changelog'а");
        }
    }
}

void MigrationRunner::InitChangelogTable(PGconn *conn) {
    const auto database_changelog_table_query =
            "CREATE TABLE IF NOT EXISTS database_changelog (version TEXT PRIMARY KEY);";
    PGresult *res_of_creation_changelog_table = PQexec(conn, database_changelog_table_query);

    const DbUtil::ResPtr guard_create_table_database_changelog(res_of_creation_changelog_table);

    if (PQresultStatus(res_of_creation_changelog_table) != PGRES_COMMAND_OK) {
        throw std::runtime_error("Failed to create changelog table");
    }
}

std::vector<std::filesystem::__cxx11::path> MigrationRunner::ReceiveMigrationFiles(
    const std::string &migrations_dir) const {
    namespace file_system = std::filesystem::__cxx11;
    std::vector<file_system::path> files;

    const std::string path_to_folder = migrations_dir.empty() ? default_path_to_migration_ : migrations_dir;

    for (auto &entry: file_system::directory_iterator(path_to_folder)) {
        if (entry.path().file_system::path::extension() == ".sql") {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    return files;
}

bool MigrationRunner::IsMigrationApplied(PGconn *conn, const std::filesystem::__cxx11::path &path) {
    const std::string version = path.filename().string();

    const char *param_values[1] = {version.c_str()};

    PGresult *res = PQexecParams(
        conn,
        "SELECT version FROM database_changelog WHERE version = $1",
        1,
        nullptr,
        param_values,
        nullptr,
        nullptr,
        0
    );

    const DbUtil::ResPtr guard_select_version(res);

    const bool is_contains_on_changelog = PQntuples(res) > 0;
    return is_contains_on_changelog;
}
