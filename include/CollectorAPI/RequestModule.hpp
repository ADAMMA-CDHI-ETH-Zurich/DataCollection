/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer, Stephan Altmüller, Francesco Feher
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

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