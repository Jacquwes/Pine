#include "api_v1_Users.h"

namespace api::v1
{
	void Users::get(HttpRequestPtr const& req, std::function<void(HttpResponsePtr const&)>&& callback)
	{
		drogon::app().getLoop()->queueInLoop(async_func(
			[=]() -> Task<>
			{
				auto db = drogon::app().getDbClient();

				try
				{
					auto result = co_await db->execSqlCoro("select count(*) from users");
					
					Json::Value json;
					json["count"] = result[0][0].as<size_t>();

					auto resp = HttpResponse::newHttpJsonResponse(json);
					resp->setStatusCode(k200OK);
					callback(resp);
				}
				catch (orm::DrogonDbException const& e)
				{
					Json::Value json;
					json["error"] = "An internal error ocurred.";

					std::cerr << "Database error: " << e.base().what() << std::endl;
				}
			}
		));
	}

	void Users::getUser(HttpRequestPtr const& req, std::function<void(HttpResponsePtr const&)>&& callback, std::string const& userId)
	{
#pragma region Id size
		if (userId.size() > 24 || userId.size() < 3)
		{
			Json::Value json;
			json["error"] = "Id is too long or too short.";

			auto res = HttpResponse::newHttpJsonResponse(json);
			res->setStatusCode(k400BadRequest);
			callback(res);
			
			return;
		}
#pragma endregion

#pragma region Invalid characters
		{
			bool err = false;

			for (auto& c : userId)
				if ((c < '-' || c > '9' || c == '/') &&
					(c < '_' || c > 'z' || c == '`'))
				{
					err = true;
					break;
				}

			if (err)
			{
				Json::Value json;
				json["error"] = "Invalid characters in id.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}
		}
#pragma endregion

		drogon::app().getLoop()->queueInLoop(async_func(
			[=]() -> Task<>
			{
				auto db = drogon::app().getDbClient();

				try
				{
					auto result = co_await db->execSqlCoro("select avataruri, backgrounduri, city, country, signature, username from users where id = $1", userId);
					
					Json::Value json;

					if (result.size() > 0)
					{
						json["avataruri"] = result[0][0].as<std::string>();
						json["backgrounduri"] = result[0][1].as<std::string>();
						json["city"] = result[0][2].as<std::string>();
						json["country"] = result[0][3].as<std::string>();
						json["id"] = userId;
						json["signature"] = result[0][4].as<std::string>();
						json["username"] = result[0][5].as<std::string>();

						auto resp = HttpResponse::newHttpJsonResponse(json);
						resp->setStatusCode(k200OK);
						callback(resp);
					}
					else
					{
						json["error"] = "Users does not exist.";

						auto resp = HttpResponse::newHttpJsonResponse(json);
						resp->setStatusCode(k404NotFound);
						callback(resp);
					}
				}
				catch (orm::DrogonDbException const& e)
				{
					Json::Value json;
					json["error"] = "An internal error ocurred.";

					std::cerr << "Database error: " << e.base().what() << std::endl;
				}
			}));

	}

	void Users::me(HttpRequestPtr const& req, std::function<void(HttpResponsePtr const&)>&& callback)
	{
		auto db = app().getDbClient();

		app().getLoop()->queueInLoop(async_func(
			[=]() -> Task<>
			{
				auto result = co_await db->execSqlCoro("select avataruri, backgrounduri, city, country, id, signature, username from users where token=$1", req->getHeader("authorization"));

				Json::Value json;

				json["avataruri"] = result[0][0].as<std::string>();
				json["backgrounduri"] = result[0][1].as<std::string>();
				json["city"] = result[0][2].as<std::string>();
				json["country"] = result[0][3].as<std::string>();
				json["id"] = result[0][4].as<std::string>();
				json["signature"] = result[0][5].as<std::string>();
				json["username"] = result[0][6].as<std::string>();

				auto resp = HttpResponse::newHttpJsonResponse(json);
				resp->setStatusCode(k200OK);
				callback(resp);
			}
		));
	}

	void Users::login(HttpRequestPtr const& req, std::function<void(HttpResponsePtr const&)>&& callback)
	{
#pragma region Validate json
		if (!req->getJsonObject())
		{
			Json::Value json;
			std::string error = req->getJsonError();
			json["error"] = error.empty() ? "* Line 0, Column 0\n  Syntax error: object or array expected.\n" : error;

			HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(k400BadRequest);
			callback(resp);

			return;
		}
#pragma endregion

		auto jsonReq = *req->getJsonObject();

#pragma region Check fields
		if (jsonReq["id"].isNull() || jsonReq["password"].isNull())
		{
			Json::Value json;
			json["error"] = "Missing field(s).";

			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(k400BadRequest);
			callback(resp);

			return;
		}
#pragma endregion

#pragma region Validate fields
		{
			std::string id = jsonReq["id"].asString();
			std::string password = jsonReq["password"].asString();

			if (id.size() > 24 || id.size() < 3)
			{
				Json::Value json;
				json["error"] = "Id is too long or too short.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

			bool err = false;

			for (auto& c : id)
				if ((c < '-' || c > '9' || c == '/') &&
					(c < '_' || c > 'z' || c == '`'))
				{
					err = true;
					break;
				}

			if (err)
			{
				Json::Value json;
				json["error"] = "Invalid characters in id.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

			err = false;

			// because it's sha512
			for (auto& c : password)
				if ((c < '0' || c > '9') &&
					(c < 'a' || c > 'z'))
				{
					err = true;
					break;
				}

			if (password.size() != 128 || err)
			{
				Json::Value json;
				json["error"] = "Invalid password.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

		}
#pragma endregion

		drogon::app().getLoop()->queueInLoop(async_func(
			[=]() -> Task<>
			{
				Json::Value json;
				auto db = drogon::app().getDbClient();

				try
				{
					auto result = co_await db->execSqlCoro("select password, token from users where id = $1;", jsonReq["id"]);
					HttpStatusCode status = k401Unauthorized;

					if (result.size() == 1)
					{
						if (jsonReq["password"] == result[0][0].as<std::string>())
						{
							json["token"] = result[0][1].as<std::string>();
							status = k200OK;
						}
						else
							json["error"] = "Incorrect password.";
					}
					else
						json["error"] = "User not found.";

					auto resp = HttpResponse::newHttpJsonResponse(json);
					resp->setStatusCode(status);
					callback(resp);

					co_return;
				}
				catch (orm::DrogonDbException const& e)
				{
					json["error"] = "An internal error ocurred.";
					auto resp = HttpResponse::newHttpJsonResponse(json);
					resp->setStatusCode(k500InternalServerError);
					callback(resp);

					co_return;
				}
			}
		));
	}

	void Users::signup(HttpRequestPtr const& req, std::function<void(HttpResponsePtr const&)>&& callback)
	{
#pragma region Validate json
		if (!req->getJsonObject())
		{
			Json::Value json;
			std::string error = req->getJsonError();
			json["error"] = error.empty() ? "* Line 0, Column 0\n  Syntax error: object or array expected.\n" : error;

			HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(k400BadRequest);
			callback(resp);

			return;
		}
#pragma endregion

		auto jsonReq = *req->getJsonObject();

#pragma region Check fields
		if (jsonReq["id"].isNull() || jsonReq["password"].isNull() || jsonReq["username"].isNull())
		{
			Json::Value json;
			json["error"] = "Missing field(s).";

			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(k400BadRequest);
			callback(resp);

			return;
		}
#pragma endregion

		std::string id = jsonReq["id"].asString();
		std::string password = jsonReq["password"].asString();
		std::string username = jsonReq["username"].asString();

#pragma region Validate fields
		{
			if (username.size() > 24 || username.size() < 3)
			{
				Json::Value json;
				json["error"] = "Username is too long or too short.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

			if (id.size() > 24 || id.size() < 3)
			{
				Json::Value json;
				json["error"] = "Id is too long or too short.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

			bool err = false;

			for (auto& c : id)
				if ((c < '-' || c > '9' || c == '/') &&
					(c < '_' || c > 'z' || c == '`'))
				{
					err = true;
					break;
				}

			if (err)
			{
				Json::Value json;
				json["error"] = "Invalid characters in id.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

			err = false;

			// because it's sha512
			for (auto& c : password)
				if ((c < '0' || c > '9') &&
					(c < 'a' || c > 'z'))
				{
					err = true;
					break;
				}

			if (password.size() != 128 || err)
			{
				Json::Value json;
				json["error"] = "Invalid password.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k400BadRequest);
				callback(res);

				return;
			}

		}
#pragma endregion

		app().getLoop()->queueInLoop(async_func(
			[=]() -> Task<>
			{
				Json::Value json;
				auto client = app().getDbClient();

				try
				{
					auto trans = client->newTransaction();

					auto res = co_await trans->execSqlCoro("select count(*) from users where id=$1;", id);
					if (res[0][0].as<size_t>() == 1)
					{
						json["error"] = "Id already exists.";

						auto resp = HttpResponse::newHttpJsonResponse(json);
						resp->setStatusCode(k400BadRequest);
						callback(resp);

						co_return;
					}

					bool unique = false;
					std::string token;
					while (!unique)
					{
						token = utils::getUuid();
						res = co_await trans->execSqlCoro("select count(*) from users where token=$1;", token);

						if (res[0][0].as<size_t>() == 0)
							unique = true;
					}

					res = co_await trans->execSqlCoro("insert into users (id, username, password, token) values ($1, $2, $3, $4);", id, username, password, token);
					
					json["token"] = token;
					auto resp = HttpResponse::newHttpJsonResponse(json);
					resp->setStatusCode(k201Created);
					callback(resp);

					co_return;
				}
				catch (orm::DrogonDbException const& e)
				{
					std::cerr << "Database exception: " << e.base().what() << std::endl;

					json["error"] = "An internal error ocurred.";

					auto res = HttpResponse::newHttpJsonResponse(json);
					res->setStatusCode(k500InternalServerError);
					callback(res);
					
					co_return;
				}
			}
		));
	}
}