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

#include "Utilities/Time.hpp"

#include "RunTime/RunTime.hpp"
#include "Serialization/Serialization.hpp"

namespace claid
{
    struct RequestDescription
    {
        //protected:
            //std::string atTime;
        public:
            std::string atTime;

            std::string what;
            int32_t period;
            std::string saveTo;
            std::string format;
            int32_t bundleNSamplesIntoOne;
        

            Reflect(RequestDescription,
                reflectMember(what);
                reflectMember(period);
                reflectMemberWithDefaultValue(saveTo, std::string(""));
                reflectMemberWithDefaultValue(format, std::string(""));
                reflectMemberWithDefaultValue(bundleNSamplesIntoOne, -1);
            )

            void getDueHourMinuteSecond(int& hour, int& minute, int& second);

        private:
            void verify(const int& hour, const int& minute, const int& second);
    };
}