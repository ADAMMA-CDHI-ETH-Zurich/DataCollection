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
        DECLARE_SERIALIZATION(Request)
        public:

            std::string dataIdentifier = "";
            int length;
            RequestDescription requestDescription;

            Request();
            Request(std::string dataIdentifier);
            Request(std::string dataIdentifier, std::shared_ptr<RequestDescription> requestDescription);

            Reflect(Request,
            
                reflectMember(dataIdentifier);
                reflectMember(length);
                reflectMember(requestDescription);
            )


            const Time getNextDueTime(); 

            Time calculateNextExecutionTime(const Time& lastExecutionDescription);       
    };
}