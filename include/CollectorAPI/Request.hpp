#pragma once

#include <string>
#include <memory>

#include "CollectorAPI/RequestDescription.hpp"
#include "Utilities/Time.hpp"

namespace portaible
{
    struct Request
    {
        public:

            std::string dataIdentifier;

            Request();
            Request(std::string dataIdentifier);
            Request(std::string dataIdentifier, std::shared_ptr<RequestDescription> requestDescription);

            template<typename Reflector>
            void reflect(Reflector& r)
            {
                r.member("RequestDescription", requestDescription, "", std::shared_ptr<RequestDescription>(new RequestDescription));
            }

            std::shared_ptr<RequestDescription> requestDescription;

            const Time getNextDueTime(); 

            Time calculateNextExecutionTime(const Time& lastExecutionDescription);



        
    };
}