#pragma once
#include "CLAID.hpp"
#include "Serialization/Serializer.hpp"
#include <fstream>

namespace claid
{
    class DataSaverModule : public claid::Module
    {
        private:
            std::shared_ptr<AbstractSerializer> serializer;

            // Where to store files. Can contain date time formats using strftime
            // E.g. /sdcard/Measurements_%d.%m.%y/%H.csv
            std::string storagePathFormat;

            std::string dataChannelName;
            
            Channel<Untyped> dataChannel;

        public:
            void initialize();
            void onData(ChannelData<Untyped> data);
            void storeData(std::vector<char>& data);
            
            std::shared_ptr<void> deserializeBinaryData(ChannelData<Untyped> data);


            Reflect(DataSaverModule, 
                reflectMember(serializer);
                reflectMember(storagePathFormat);
                reflectMember(dataChannelName);
            )
    };
}