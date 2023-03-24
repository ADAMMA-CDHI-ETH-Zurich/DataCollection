#pragma once

#include "Serialization/AbstractSerializer.hpp"
#include "CLAID.hpp"
#include "Utilities/Path.hpp"

#include <fstream>

namespace claid
{
    class DataSaverModule;

    class FileSaver
    {
        private:
            std::shared_ptr<AbstractSerializer> serializer;
            std::string what;
            std::string storagePath;
            std::string fileNameFormat;
            bool excludeHeader;
            
            std::string tmpStoragePath;
            
            DataSaverModule* parentModule = nullptr;
            Path lastPath;
            std::ofstream file;
            Channel<Untyped> dataChannel;

        public:

            Reflect(FileSaver,
                reflectMember(serializer);
                reflectMember(what);
                reflectMember(storagePath);
                reflectMember(fileNameFormat);
                reflectMemberWithDefaultValue(excludeHeader, false);
                reflectMemberWithDefaultValue(tmpStoragePath, std::string(""));
            )


            void initialize(DataSaverModule* parentModule);
            void onData(ChannelData<Untyped> data);
            void storeData(std::vector<char>& data);
            void storeDataHeader(std::vector<char>& dataHeader);
    
            void getCurrentPathRelativeToStorageFolder(Path& path);
            void createDirectoriesRecursively(const std::string& path);
            void openFile(const std::string& path);
            
            void moveTemporaryFilesToStorageDestination();

            void createStorageFolder(const Path& currentSavePath);
            void createTmpFolderIfRequired(const Path& currentSavePath);


    };
}