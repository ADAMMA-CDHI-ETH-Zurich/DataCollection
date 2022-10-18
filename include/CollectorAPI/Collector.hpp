#pragma once

#include <string>
#include <map>
#include <functional>

#include "Channel/Channel.hpp"
#include "Request.hpp"

#include "RunTime/RunTime.hpp"

namespace claid
{
    namespace CollectorAPI
    {
        class Collector : public claid::Module
        {
            protected:
                typedef std::function<void(const Request&)> Callback;

            private:

                Channel<Request> requestChannel;
                std::map<std::string, Callback> callbacks;

            protected:
                void registerDataRequestHandler(std::string dataIdentifier, Callback callback);

                virtual void onRequest(ChannelData<Request> request);
                virtual void initializeCollector() {}

                void initialize();


        };
    }
}

#define RequestCallback(function) std::bind(&function, this, std::placeholders::_1)

#define DECLARE_COLLECTOR(className)\
    DECLARE_MODULE(className) \

#define REGISTER_COLLECTOR(className)\
    REGISTER_MODULE(className)\


    