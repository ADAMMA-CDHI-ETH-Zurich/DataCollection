#pragma once

#include <string>
#include <map>
#include <functional>

#include "Channel/Channel.hpp"
#include "Request.hpp"
#include "CollectorFactory/CollectorFactory.hpp"

#include "RunTime/RunTime.hpp"

namespace portaible
{
    namespace CollectorAPI
    {
        class Collector : public portaible::Module
        {
            protected:
                typedef std::function<void(const Request&)> Callback;

            private:

                Channel<Request> requestChannel;
                std::map<std::string, Callback> callbacks;

            protected:
                void registerDataRequestHandler(std::string dataIdentifier, Callback callback);

                virtual void onRequest(ChannelRead<Request> request);
                virtual void initializeCollector() {}

                void initialize();


        };
    }
}

#define RequestCallback(function) std::bind(&function, this, std::placeholders::_1)

#define DECLARE_COLLECTOR(className)\
    PORTAIBLE_MODULE(className) \
    DECLARE_COLLECTOR_FACTORY(className) \

#define REGISTER_COLLECTOR(className)\
    PORTAIBLE_SERIALIZATION(className)\
    REGISTER_TO_COLLECTOR_FACTORY(className) 