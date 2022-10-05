#pragma once

#include <string>
#include <memory>

#include "CollectorAPI/RequestDescription.hpp"
#include "Utilities/Time.hpp"
#include "Reflection/Reflect.hpp"

namespace portaible
{
    struct Request
    {
        DECLARE_SERIALIZATION(Request)
        public:

            std::string dataIdentifier = "AudioData";
            int length;

            Request();
            Request(std::string dataIdentifier);
            Request(std::string dataIdentifier, std::shared_ptr<RequestDescription> requestDescription);

            Reflect(Request,
            
                reflectMember(dataIdentifier);
                reflectMember(length);
            )

            std::shared_ptr<RequestDescription> requestDescription;

            const Time getNextDueTime(); 

            Time calculateNextExecutionTime(const Time& lastExecutionDescription);       
    };
}