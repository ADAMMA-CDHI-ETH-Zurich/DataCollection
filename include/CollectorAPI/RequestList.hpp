#pragma once

#include <vector>

#include "Request.hpp"


// SplitReflect needs to be included after (!!) defining reflectRead and reflectWrite for the type ! 
#include "Reflection/SplitReflect.hpp"



namespace portaible
{
    class RequestList
    {
        public:
                   
            template<typename Reflector>
            void reflect(Reflector& r)
            {
                splitReflect(r, *this);
            }

            std::vector<Request> data;
    };
}

namespace portaible
{
    // CAUTION: READ MEANS READ FROM "DATA" (NOT FROM FILE) -> SERIALIZATION
    template<typename Reflector>
    void reflectRead(Reflector& r, RequestList& requestList)
    {
        // ONLY USE BEGIN AND ENDSEQUENCE IF EVERY ELEMENT
        // HAS THE SAME NAME, LIKE "item"
        // r.beginSequence();
        size_t count;
        // Set number of elements
        size_t size = requestList.data.size();
        r.countElements(size);

        for(size_t i = 0; i < requestList.data.size(); i++)
        {
            Request& request = requestList.data[i];
            r.enforceName(request.dataIdentifier, i);
            r.member(request.dataIdentifier.c_str(), request, "");
        }

        //r.endSequence();
    }

    // CAUTION: WRITE MEANS WRITE TO "DATA" (NOT TO FILE) -> DESERIALIZATION
    template<typename Reflector>
    void reflectWrite(Reflector& r, RequestList& requestList)
    {
        // ONLY USE BEGIN AND ENDSEQUENCE IF EVERY ELEMENT
        // HAS THE SAME NAME, LIKE "item"
        // r.beginSequence();
        size_t count;
        // Gets count from the current data.
        r.countElements(count);

        requestList.data.resize(count);

        std::string requestDataIdentifier;

        for(size_t i = 0; i < count; i++)
        {
            Request& request = requestList.data[i];
            r.enforceName(requestDataIdentifier, i);
            r.member(requestDataIdentifier.c_str(), request, "");
            request.dataIdentifier = requestDataIdentifier;
        }
        // ONLY USE BEGIN AND ENDSEQUENCE IF EVERY ELEMENT
        // HAS THE SAME NAME, LIKE "item"
        // r.endSequence();
    }
}
