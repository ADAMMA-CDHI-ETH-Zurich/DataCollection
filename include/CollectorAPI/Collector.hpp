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


    