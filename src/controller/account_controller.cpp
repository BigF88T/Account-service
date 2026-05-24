//
// Created by thbr on 23.05.2026.
//

#include "crow.h"
#include "account_controller.h"

void AccountController::RegisterRoutes(crow::SimpleApp& app) {

    CROW_ROUTE(app, "/account/<int>")
    ([this](int id) {
        if (const auto account = account_service_.FindAccount(id)) {
            crow::json::wvalue response;
            response["id"] = account->id;
            response["name"] = account->owner_name;
            response["balance"] = account->balance;
            return crow::response(200, response);
        }
        return crow::response(404);
    });

    CROW_ROUTE(app, "/account").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        const auto json = crow::json::load(req.body);

        if (!json || !json.has("name")) {
            return crow::response(400, "Invalid JSON: 'name' field is required");
        }

        const std::string name = json["name"].s();
        const auto new_account = account_service_.CreateAccount(name);

        crow::json::wvalue response_json;
        response_json["id"] = new_account.id;
        response_json["name"] = new_account.owner_name;
        response_json["balance"] = new_account.balance;

        return crow::response(201, response_json);
    });

    CROW_ROUTE(app, "/account/<int>/deposit").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req, int id) {
        const auto json = crow::json::load(req.body);

        if (!json || !json.has("amount")) {
            return crow::response(400, "Invalid JSON: 'amount' field is required");
        }

        const auto amount = static_cast<float>(json["amount"].d());

        const bool is_deposit_credited = account_service_.Deposit(id, amount);

        if (is_deposit_credited) {
            return crow::response(200);
        }

        return crow::response(400);
    });

    CROW_ROUTE(app, "/account/transfer").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        const auto json = crow::json::load(req.body);

        if (!json || !json.has("from") || !json.has("to") || !json.has("amount")) {
            return crow::response(400, "Invalid JSON: fields: 'from', 'to', 'amount' are required");
        }

        const int from_id = static_cast<int>(json["from"].i());
        const int to_id = static_cast<int>(json["to"].i());
        const double amount = static_cast<float>(json["amount"].d());

        if (amount <= 0) {
            return crow::response(400, "Amount must be positive");
        }

        const bool is_transfer_complete = account_service_.Transfer(from_id, to_id, amount);

        if (is_transfer_complete) {
            return crow::response(200);
        }

        return crow::response(400);
    });
}
