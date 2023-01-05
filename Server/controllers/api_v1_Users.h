#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
	namespace v1
	{
		class Users : public drogon::HttpController<Users>
		{
		public:
			METHOD_LIST_BEGIN
				METHOD_ADD(Users::get, "/", Get, "api::v1::LoggedInFilter");
				METHOD_ADD(Users::getUser, "/{}", Get, "api::v1::LoggedInFilter");
				METHOD_ADD(Users::me, "/me", Get, "api::v1::LoggedInFilter");
				METHOD_ADD(Users::login, "/login", Post);
				METHOD_ADD(Users::signup, "/signup", Post);
			METHOD_LIST_END;



			void get(HttpRequestPtr const& req,
					 std::function<void(HttpResponsePtr const&)>&& callback);

			void getUser(HttpRequestPtr const& req,
						 std::function<void(HttpResponsePtr const&)>&& callback,
						 std::string const& userId);

			void me(HttpRequestPtr const& req,
					std::function<void(HttpResponsePtr const&)>&& callback);

			void login(HttpRequestPtr const& req,
					   std::function<void(HttpResponsePtr const&)>&& callback);

			void signup(HttpRequestPtr const& req,
						std::function<void(HttpResponsePtr const&)>&& callback);
		};
	}
}
