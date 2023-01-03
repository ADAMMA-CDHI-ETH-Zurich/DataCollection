#pragma once

#include "CLAID.hpp"
#include "RequestDescription.hpp"
#include "Request.hpp"
#include "Utilities/FileUtils.hpp"

namespace claid
{
    class RequestModule;


    class RequestHandler
    {
        private:
            RequestDescription requestDescription;

            Channel<Untyped> dataChannel;

            RequestModule* parent = nullptr;

            int existingRecordingsInDirectory = 0;
            int numSamples = 0;
            const int handlerID;


            std::string getCurrentRecordingPath() const;
            std::string getPeriodicFunctionName() const;
            void setupStorageFolder();
            void setupRequest();

            void onData(ChannelData<Untyped> data);

            void periodicRequest();

        public:

            RequestHandler(const int handlerID);

            RequestHandler(RequestModule* parent, const RequestDescription& requestDescription, const int handlerID);

            void initialize();
            void terminate();
        
            

            

    };
}