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
            std::string atTime;
            RepetitionDescription repetitionDecription;


        public:
            void getDueHourMinuteSecond(int& hour, int& minute, int& second);

            Reflect(RequestDescription,
                reflectMember(atTime);
                reflectMember(repetitionDecription);
       //         r.member("RepetitionDescription", repetitionDecription, "Describes whether to repeat requests after a given time.");
            )

        private:
            void verify(const int& hour, const int& minute, const int& second);
    };
}