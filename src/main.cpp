#include "account_controller.h"
#include "account_service_db_impl.h"
#include <cstdlib>
#include <string>
#include "crow/app.h"

int main() {
    crow::SimpleApp app;

    const char* env_url = std::getenv("DATABASE_URL");
    const auto default_db_url = "postgresql://postgres:postgres@localhost:5444/account-service";

    const std::string db_url = (env_url != nullptr) ? env_url : default_db_url;

    AccountServiceDbImpl account_service(db_url);
    AccountController account_controller(account_service);

    account_controller.RegisterRoutes(app);

    app.port(18080).multithreaded().run();

    return 0;
}
