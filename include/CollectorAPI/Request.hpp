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
#include <memory>

#include "CollectorAPI/RequestDescription.hpp"
#include "Utilities/Time.hpp"
#include "Reflection/Reflect.hpp"

namespace claid
{
    struct Request
    {
        public:

            std::string dataIdentifier = "";
            int length;

            Request();
            
            Reflect(Request,
                reflectMember(dataIdentifier);
                reflectMember(length);
            )


            // const Time getNextDueTime(); 

            // Time calculateNextExecutionTime(const Time& lastExecutionDescription);       
    };
}

