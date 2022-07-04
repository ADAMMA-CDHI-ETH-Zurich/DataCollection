#pragma once

#include <string>

#include "Utilities/Time.hpp"
#include "RepetitionDescription.hpp"

#include "RunTime/RunTime.hpp"
#include "Serialization/Serialization.hpp"

namespace portaible
{
    struct RequestDescription
    {
        DECLARE_SERIALIZATION(RequestDescription)
        //protected:
            std::string atTime;
            RepetitionDescription repetitionDecription;


        public:
            void getDueHourMinuteSecond(int& hour, int& minute, int& second);

            template<typename Reflector>
            void reflect(Reflector& r)
            {
                r.member("AtTime", atTime, "Time execute the request.");
            }

        private:
            void verify(const int& hour, const int& minute, const int& second);
    };
}