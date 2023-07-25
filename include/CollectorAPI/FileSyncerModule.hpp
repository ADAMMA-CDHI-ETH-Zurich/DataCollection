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
        
        private:
            // On this channel, we post all the files available
            Channel<std::vector<std::string>> completeFileListChannel;

            // On this channel, the FileReceiver can request files that we will send.
            Channel<std::string> requestedFileChannel;

            // On this channel, we post the actual files.
            Channel<DataFile> dataFileChannel;

            // On this channel, the FileReceiver sends the name of the file that it received (acknowledgement).
            Channel<std::string> receivedFileAcknowledgementChannel;

            std::string filePath;

            Time lastMessageFromFileReceiver;
        
            std::string completeFileListChannelName;
            std::string requestedFileChannelName;
            std::string dataFileChannelName;
            std::string receivedFilesAcknowledgementChannelName;

            int syncingPeriodInMs;
            bool deleteFileAfterSync;

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
                    return;
                    //CLAID_THROW(Exception, "Error in FileSyncerModule, cannot scan directory \"" << this->filePath << "\" "
                    //<< "for files. Directory either does not exist or we do not have reading permissions.");
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
                Logger::printfln("Sending file list");
            }

            void sendRequestedFile(const std::string& relativeFilePath)
            {
                printf("Requested file %s\n", relativeFilePath.c_str());

                DataFile file;
                std::string path = this->filePath + std::string("/") + relativeFilePath;
                if(!file.loadFromPath(path))
                {
                    return;
                    // CLAID_THROW(Exception, "Error in FileSyncerModule, file \"" << path << "\" was requested to be sent,\n"
                    // << "but it could not be loaded from the filesystem. Either the file does not exist or we do not have permission to read it.");
                }
                file.relativePath = relativeFilePath;
                this->dataFileChannel.post(file);
            }

            void onFileRequested(ChannelData<std::string> missingFileData)
            {
                printf("Requested file\n");   
                this->lastMessageFromFileReceiver = Time::now();
                sendRequestedFile(missingFileData->value());
            }

            void onFileReceivalAcknowledged(ChannelData<std::string> receivedFile)
            {
                this->lastMessageFromFileReceiver = Time::now();
                if(this->deleteFileAfterSync)
                {
                    Logger::printfln("Received acknowledgement of file received %s, deleting it.", receivedFile->value().c_str());
                    std::string path = this->filePath + std::string("/") + receivedFile->value();
                    Logger::printfln("Deleting %s", path.c_str());
                    FileUtils::removeFileIfExists(path);
                }
            }

            size_t millisecondsSinceLastMessageFromFileReceiver()
            {
                size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>((Time::now() - lastMessageFromFileReceiver)).count();
                
                return diff;
            }

            void periodicSync()
            {
                // If a previous syncing process is still going on (e.g., file receiver is still requesting files),
                // do not start a new syncing process just yet.
                if(millisecondsSinceLastMessageFromFileReceiver() >= this->syncingPeriodInMs / 2)
                {
                    Logger::printfln("!!!PERIODIC SYNC!!!");
                    sendFileList();
                }
                
            }

        public:
            Reflect(FileSyncerModule,
                reflectMember(filePath);
                reflectMember(syncingPeriodInMs);
                reflectMemberWithDefaultValue(deleteFileAfterSync, false);

                reflectMemberWithDefaultValue(completeFileListChannelName, std::string("FileSyncer/CompleteFileList"));
                reflectMemberWithDefaultValue(requestedFileChannelName, std::string("FileSyncer/RequestedFileList"));
                reflectMemberWithDefaultValue(dataFileChannelName, std::string("FileSyncer/DataFiles"));
                reflectMemberWithDefaultValue(receivedFilesAcknowledgementChannelName, std::string("FileSyncer/ReceivedFilesAcknowledgement"));
            )

            void initialize()
            {
                #ifdef __APPLE__
                    #if TARGET_OS_IPHONE
                        this->filePath = iOSApplicationPathHelper::getAppDocumentsPath() + std::string("/") + this->filePath;
                    #endif
                #endif

                this->completeFileListChannel = this->publish<std::vector<std::string>>(this->completeFileListChannelName);
                this->requestedFileChannel = this->subscribe<std::string>(this->requestedFileChannelName, &FileSyncerModule::onFileRequested, this);
                this->dataFileChannel = this->publish<DataFile>(this->dataFileChannelName);

                 
                this->receivedFileAcknowledgementChannel = this->subscribe<std::string>(this->receivedFilesAcknowledgementChannelName, &FileSyncerModule::onFileReceivalAcknowledged, this);
                
                this->lastMessageFromFileReceiver = Time::now();
                this->registerPeriodicFunction("PeriodicSyncFunction", &FileSyncerModule::periodicSync, this, this->syncingPeriodInMs, true);
            }

            void postInitialize()
            {
                // Call sync at startup once already.
                this->periodicSync();
            }

             

    };
}