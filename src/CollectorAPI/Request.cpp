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

#include "CollectorAPI/Request.hpp"

namespace claid
{
    Request::Request()
    {
       // this->requestDescription = std::shared_ptr<RequestDescription>(new RequestDescription); 
    }
 
    // Request::Request(std::string dataIdentifier) : dataIdentifier(dataIdentifier)
    // {
    //   //  this->requestDescription = std::shared_ptr<RequestDescription>(new RequestDescription); 

    // }


    // // Request::Request(std::string dataIdentifier, std::shared_ptr<RequestDescription> requestDescription) : requestDescription(requestDescription)
    // // {

    // // }



    // const Time Request::getNextDueTime()
    // {
    //     // Change later for saved values;
    //     Time lastExecutionTime = Time::currentDay();
    //     bool hasEverBeenExecuted = true;

    //     int hour, minute, second;
    //     requestDescription.getDueHourMinuteSecond(hour, minute, second);

       
        
    //     if(!hasEverBeenExecuted)
    //     {
    //         // Schedule execution today at specified time.

    //         // TODO: What if todayAt is already past ? 
    //         return Time::todayAt(hour, minute, second);
    //     }

    //     if(this->requestDescription.repetitionDecription.type == RepetitionDescription::Type::NEVER)
    //     {
    //         return Time::unixEpoch();
    //     }

    //     return calculateNextExecutionTime(lastExecutionTime);
        


    // }

    // Time Request::calculateNextExecutionTime(const Time& lastExecutionTime)
    // {
    //     Time now = Time::now();
    //     Time nextExecutionTime = lastExecutionTime;

    //     int secondsToIncrease = this->requestDescription.repetitionDecription.hours * 60 * 60 +
    //                                 this->requestDescription.repetitionDecription.minutes * 60 +
    //                                     this->requestDescription.repetitionDecription.seconds;

    //     // Calculate next execution.
    //     while (nextExecutionTime < now)
    //     {
    //         nextExecutionTime += std::chrono::seconds(secondsToIncrease);
    //     } 

    //     return nextExecutionTime;
    // }

}

REGISTER_SERIALIZATION(claid::Request);