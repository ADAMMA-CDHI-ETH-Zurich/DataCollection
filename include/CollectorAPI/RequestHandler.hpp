#pragma once

#include "CLAID.hpp"
#include "RequestDescription.hpp"
#include "Request.hpp"
#include "Utilities/FileUtils.hpp"

#ifdef __APPLE__
	#include "TargetConditionals.h"
#endif

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

            #ifdef __APPLE__
                #if TARGET_OS_IPHONE
                // Changes this->filePath from e.g., Measurements/AudioData to 
                // /var/mobile/Containers/Data/Application/{iOS_APP_ID}/Documents/Measurements/AudioData
                // (path and iOS_APP_ID are determined automatically using iOSApplicationPathHelper).
                void prependiOSDocumentsPathToFilePath();
                #endif
            #endif

        public:

            RequestHandler(const int handlerID);

            RequestHandler(RequestModule* parent, const RequestDescription& requestDescription, const int handlerID);

            void initialize();
            void terminate();
            
        
            

            

    };
}