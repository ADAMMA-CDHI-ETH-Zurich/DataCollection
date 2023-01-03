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
        private:
            std::string atTime;

            std::string what;
            std::string period;
            std::string saveTo;
            std::string format;


        public:

            Reflect(RequestDescription,
                reflectMember(what);
                reflectMember(period);
                reflectMember(saveTo);
                reflectMember(format);
            )

            void getDueHourMinuteSecond(int& hour, int& minute, int& second);

        private:
            void verify(const int& hour, const int& minute, const int& second);
    };
}