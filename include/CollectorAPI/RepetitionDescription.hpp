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
namespace claid
{
    struct RepetitionDescription
    {
        /*  Specifies when to repeat a data request after the previous request.
            E.g. 2.5hrs after the last request finished 
                For Collectors that record data over time, this means the request is repeated 2.5 hrs AFTER the recording has FINISHED.
            
            Or: 20 minutes after the last request was issued.
                For Collectors that record data over time, this means the request is repeated 2.5hrs after the recording has STARTED.
        */

       enum Type
       {
           AFTER_FINISHED,
           AFTER_START,
           NEVER
       };

        int seconds = 42;
        int minutes = 10;
        int hours = 0;
        Type type = NEVER;

        Reflect(RepetitionDescription,
            reflectMember(seconds);
            reflectMember(minutes);
            reflectMember(hours);
        )
    
    };
}