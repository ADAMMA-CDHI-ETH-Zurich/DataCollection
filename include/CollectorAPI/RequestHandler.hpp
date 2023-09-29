/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer, Stephan Altmüller, Francesco Feher
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

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

            std::vector<ChannelData<Untyped>> dataBuffer;

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


            void storeBinaryDataSample(TaggedData<BinaryData> binaryData, const std::string& suffix);
            void storeXMLDataSample(std::shared_ptr<XMLNode> xml, const std::string& suffix);
            void updateDataBundle();


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