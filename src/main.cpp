#include "account_controller.h"
#include "account_service_db_impl.h"
#include <string>

#include "audit_repository.h"
#include "configuration.h"
#include "migration_runner.h"
#include "crow/app.h"

int main() {
    const Configuration configuration("resources/application.json");

    const MigrationRunner migration_runner(configuration.GetDbUrl());
    migration_runner.RunMigration(configuration.GetMigrationsDir());

    crow::SimpleApp app;

    constexpr AccountRepository account_repository;
    constexpr AuditRepository audit_repository;
    AccountServiceDbImpl account_service(
        account_repository,
        audit_repository,
        configuration.GetDbUrl()
    );
    TransactionFacade transaction_facade(account_service);

    const AccountController account_controller(
        transaction_facade,
        account_service
    );
    account_controller.RegisterRoutes(app);

    app.port(configuration.GetPort()).multithreaded().run();

    return 0;
}
