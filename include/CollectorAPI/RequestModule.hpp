#pragma once

#include "CLAID.hpp"

#include "CollectorAPI/RequestDescription.hpp"
#include "StandardReflectors/ListReflector.hpp"
#include "RequestHandler.hpp"

namespace claid
{
    class RequestModule : public Module
    {
        friend class RequestHandler;


        private:
            std::vector<RequestDescription> requestDescriptions;
            std::vector<RequestHandler*> requestHandlers;
            
            std::string requestChannelName;
            Channel<Request> requestChannel;

            void postRequest(Request& request)
            {
                this->requestChannel.post(request);
            }


        public:
            Reflect(RequestModule,
                ListReflector<std::vector<RequestDescription>>("Request", this->requestDescriptions).reflect(reflector);
                reflectMember(requestChannelName);
            )

            
            void postInitialize()
            {
                Logger::printfln("RequestModule begin init %d", this->requestDescriptions.size());
                this->requestChannel = this->publish<Request>(this->requestChannelName);

                for(const RequestDescription& description : this->requestDescriptions)
                {
                    int handlerID = this->requestHandlers.size();
                    RequestHandler* handler = new RequestHandler(this, description, handlerID);
                    handler->initialize();

                    this->requestHandlers.push_back(handler);
                }
                Logger::printfln("RequestModule end init");
            }

            void terminate()
            {
                for(RequestHandler* requestHandler : this->requestHandlers)
                {
                    requestHandler->terminate();
                    delete requestHandler;
                }
            }
    };
}