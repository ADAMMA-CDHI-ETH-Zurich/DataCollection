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