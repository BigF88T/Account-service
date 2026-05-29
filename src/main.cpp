#include "account_controller.h"
#include "account_service_db_impl.h"
#include <cstdlib>
#include <string>

#include "configuration.h"
#include "migration_runner.h"
#include "crow/app.h"

int main() {
    const Configuration configuration("resources/application.json");

    const MigrationRunner migration_runner(configuration.GetDbUrl());
    migration_runner.RunMigration(configuration.GetMigrationsDir());

    crow::SimpleApp app;

    AccountServiceDbImpl account_service(configuration.GetDbUrl());

    AccountController account_controller(account_service);
    account_controller.RegisterRoutes(app);

    app.port(configuration.GetPort()).multithreaded().run();

    return 0;
}
