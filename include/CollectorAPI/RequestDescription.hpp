#pragma once

#include <string>

#include "Utilities/Time.hpp"
#include "RepetitionDescription.hpp"

#include "RunTime/RunTime.hpp"
#include "Serialization/Serialization.hpp"

namespace claid
{
    struct RequestDescription
    {
        DECLARE_SERIALIZATION(RequestDescription)
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