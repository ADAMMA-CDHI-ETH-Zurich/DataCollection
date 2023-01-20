#pragma once

#include "CLAID.hpp"
#include "Utilities/FileUtils.hpp"
#include "Utilities/StringUtils.hpp"
#include "DataFile.hpp"
#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
    #include "CollectorAPI/iOSHelper/iOSApplicationPathHelper.hpp"
    #endif
#endif


namespace claid
{
    // Syncs files contained in a certain directory with a FileReceiverModule.
    // This works in multiple steps:
    // 1st The FileSyncerModule scans the specified directory and builds a list of available files.
    // 2nd This list is posted to a channel that the FileReceiverModule listens on.
    // 3rd The FileReceiverModule build a list of available files in the target directory aswell and compares
    // it with the list received from the FileSyncerModule.
    // 4th The FileReceiverModule sends back a list of the files that are missing on it's side; this list is a subset
    // of the original list containing all files sent by the FileSyncerModule.
    // 5th The FileSyncerModule posts each file (in binary format) to the dataChannel, along with the target path.
    // 6th The FileReceiverModule saves each file.
    class FileSyncerModule : public Module
    {
        DECLARE_MODULE(FileSyncerModule)
        
        private:
            // On this channel, we post all the files available
            Channel<std::vector<std::string>> completeFileListChannel;

            // On this channel, we receive the list of files
            // that are missing on the receiver files.
            // This will be a subset of the list send in the completeFileListChannel.
            Channel<std::vector<std::string>> requestedFileListChannel;

            // On this channel, we post the actual files.
            Channel<DataFile> dataFileChannel;

            std::string filePath;

        
            std::string completeFileListChannelName;
            std::string requestedFileListChannelName;
            std::string dataFileChannelName;

            int syncingPeriodInMs;

            void makePathRelative(std::string& path)
            {
                std::string basePath = this->filePath;
                if(this->filePath[this->filePath.size() - 1] != '/')
                {
                    basePath += '/';
                }

                if(StringUtils::startsWith(path, basePath))
                {
                    path = path.substr(basePath.size(), path.size());   
                }
            }

            void buildFileList(std::vector<std::string>& fileList)
            {
                fileList.clear();
                if(!FileUtils::getAllFilesInDirectoryRecursively(this->filePath, fileList))
                {
                    CLAID_THROW(Exception, "Error in FileSyncerModule, cannot scan directory \"" << this->filePath << "\" "
                    << "for files. Directory either does not exist or we do not have reading permissions.");
                }

                for(std::string& path : fileList)
                {
                    makePathRelative(path);
                }

            }

            void sendFileList()
            {
                std::vector<std::string> fileList;
                this->buildFileList(fileList);
                this->completeFileListChannel.post(fileList);
            }

            void sendRequestedFiles(const std::vector<std::string>& filesToSend)
            {
                for(const std::string& relativePath : filesToSend)
                {
                                    printf("Requested file %s\n", relativePath.c_str());

                    DataFile file;
                    std::string path = this->filePath + std::string("/") + relativePath;
                    if(!file.loadFromPath(path))
                    {
                        CLAID_THROW(Exception, "Error in FileSyncerModule, file \"" << path << "\" was requested to be sent,\n"
                        << "but it could not be loaded from the filesystem. Either the file does not exist or we do not have permission to read it.");
                    }
                    file.relativePath = relativePath;
                    this->dataFileChannel.post(file);
                }
            }

            void onFilesRequested(ChannelData<std::vector<std::string>> missingFileListData)
            {
                printf("Requested files\n");   
                const std::vector<std::string>& missingFileList = missingFileListData->value();
                
                sendRequestedFiles(missingFileList);
            }

            void periodicSync()
            {
                Logger::printfln("!!!PERIODIC SYNC!!!");
                sendFileList();
            }

        public:
            Reflect(FileSyncerModule,
                reflectMember(filePath);
                reflectMember(syncingPeriodInMs);

                reflectMemberWithDefaultValue(completeFileListChannelName, std::string("FileSyncer/CompleteFileList"));
                reflectMemberWithDefaultValue(requestedFileListChannelName, std::string("FileSyncer/RequestedFileList"));
                reflectMemberWithDefaultValue(dataFileChannelName, std::string("FileSyncer/DataFiles"));
            )

            void initialize()
            {
                #ifdef __APPLE__
                    #if TARGET_OS_IPHONE
                        this->filePath = iOSApplicationPathHelper::getAppDocumentsPath() + std::string("/") + this->filePath;
                    #endif
                #endif

                this->completeFileListChannel = this->publish<std::vector<std::string>>(this->completeFileListChannelName);
                this->requestedFileListChannel = this->subscribe<std::vector<std::string>>(this->requestedFileListChannelName, &FileSyncerModule::onFilesRequested, this);
                this->dataFileChannel = this->publish<DataFile>(this->dataFileChannelName);

                this->registerPeriodicFunction("PeriodicSyncFunction", &FileSyncerModule::periodicSync, this, this->syncingPeriodInMs);
            }

            void postInitialize()
            {
                // Call sync at startup once already.
                this->periodicSync();
            }

             

    };
}