#include "CollectorAPI/Collector.hpp"

namespace portaible
{
    namespace CollectorAPI
    {
        void Collector::registerDataRequestHandler(std::string dataIdentifier, Callback callback)
        {
            this->callbacks.insert(std::make_pair(dataIdentifier, callback));
        }

        void Collector::onRequest(ChannelRead<Request> request)
        {
            const std::string& identifier = request->value().dataIdentifier;

            auto it = this->callbacks.find(identifier);
            if(it != this->callbacks.end())
            {
                it->second(request->value());
            }
        }

        void Collector::initialize()
        {
            this->initializeCollector();
        }
    }
}