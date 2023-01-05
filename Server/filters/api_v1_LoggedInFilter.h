/**
 *
 *  api_v1_LoggedInFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;
namespace api
{
    namespace v1
    {

        class LoggedInFilter : public HttpFilter<LoggedInFilter>
        {
        public:
            LoggedInFilter() {}
            void doFilter(const HttpRequestPtr& req,
                          FilterCallback&& fcb,
                          FilterChainCallback&& fccb) override;
        };

    }
}