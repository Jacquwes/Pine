/**
 *
 *  api_v1_LoggedInFilter.cc
 *
 */

#include "api_v1_LoggedInFilter.h"

#include "drogon/drogon.h"

using namespace drogon;
using namespace api::v1;

void LoggedInFilter::doFilter(const HttpRequestPtr &req,
						 FilterCallback &&fcb,
						 FilterChainCallback &&fccb)
{
				Json::Value json;

			try
			{
				std::string token = req->getHeader("authorization");

				if (token.empty())
				{
					json["error"] = "Missing authentication token.";
					
					auto res = HttpResponse::newHttpJsonResponse(json);
					res->setStatusCode(k401Unauthorized);
					fcb(res); 
					
					return;
				}

				auto db = app().getDbClient();

				auto result = db->execSqlSync("select count(*) from users where token=$1", token);

				if (result[0][0].as<size_t>() == 1)
				{
					fccb();
				}
				else
				{
					json["error"] = "Incorrect authentication token.";
					
					auto res = HttpResponse::newHttpJsonResponse(json);
					res->setStatusCode(k401Unauthorized);
					fcb(res);
				}
			}
			catch (orm::DrogonDbException const& e)
			{
				std::cerr << "Database exception: " << e.base().what() << std::endl;

				json["error"] = "An internal error ocurred.";

				auto res = HttpResponse::newHttpJsonResponse(json);
				res->setStatusCode(k500InternalServerError);
				fcb(res);

			}
}
