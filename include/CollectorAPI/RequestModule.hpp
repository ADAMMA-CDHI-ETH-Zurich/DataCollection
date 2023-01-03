#pragma once

#include "CLAID.hpp"

#include "CollectorAPI/RequestDescription.hpp"
#include "StandardReflectors/ListReflector.hpp"

namespace claid
{
    class RequestModule : public Module
    {
        DECLARE_MODULE(RequestModule)

        private:
            std::vector<RequestDescription> requestDescriptionList;

        public:
            Reflect(RequestModule,
                ListReflector<std::vector<RequestDescription>>("Request", this->requestDescriptionList).reflect(reflector);

            )

            void initialize()
            {
                std::cout << "LOADED " << requestDescriptionList.size() << " requests \n";
            }
    };
}