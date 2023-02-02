#pragma once
#include "CLAID.hpp"
#include "XML/XMLDocument.hpp"
#include "Utilities/FileUtils.hpp"
#include "Utilities/StringUtils.hpp"
#include "DataFile.hpp"
#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
    #include "CollectorAPI/iOSHelper/iOSApplicationPathHelper.hpp"
    #endif
#endif
#include <queue>
namespace claid
{
    // Counterpart to FileSyncerModule.
    // Receives a list of files and compares it with files available in a specified directory.
    // All files that are missing are requested from the FileSyncerModule, which will then be received
    // and stored in the path.
    class FileReceiverModule : public Module
    {
        DECLARE_MODULE(FileReceiverModule)

        private:

            // On this channel, we receive all the files available
            Channel<std::vector<std::string>> completeFileListChannel;

            // On this channel, we post files that are missing.
            // They will be send by the FileSyncerModule.
            Channel<std::string> requestedFileChannel;

            // On this channel, we receive files from the FileSyncerModule.
            Channel<DataFile> dataFileChannel;

            // On this channel, we acknowledge received files.
            // If "deleteFilesAfterSync" is activated in FileSyncerModule, it will delete the file locally.
            Channel<std::string> receivedFilesAcknowledgementChannel;

            std::string filePath;
            std::string completeFileListChannelName;
            std::string requestedFileChannelName;
            std::string dataFileChannelName;
            std::string receivedFilesAcknowledgementChannelName;

            bool storeArrivalTimePerFile = false;

            std::queue<std::string> missingFilesQueue;

            template<typename T>
            bool isElementContainedInVector(const std::vector<T>& vector, const T& element)
            {
                return std::find(vector.begin(), vector.end(), element) != vector.end();
            }

            // Get elements that are contained in both lists.
            void getMissingElements(
                    const std::vector<std::string>& completeList, 
                    std::vector<std::string>& missingList)
            {
                missingList.clear();
                std::string path;
                for(const std::string& fileName : completeList)
                {   
                    path = this->filePath + std::string("/") + fileName;
                    if(!FileUtils::fileExists(path))
                    {
                        missingList.push_back(fileName);
                    }
                }
            }

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


            // Splits a/b/c/d.txt into a/b/c and d.txt
            void splitPathIntoFolderAndFileName(const std::string& path, std::string& folderPath, std::string& fileName)
            {
                if(path.find("/") == std::string::npos)
                {
                    // No folder, assume path is only a fileName.
                    folderPath = "";
                    fileName = "path";
                    return;
                }
                int index = path.size() - 1;
                while(index > 0)
                {
                    if(path[index] == '/')
                    {
                        folderPath = path.substr(0, index);
                        fileName = path.substr(index + 1, path.size());
                        return;
                    }
                    index--;
                }
            }

            void getListOfFilesInTargetDirectory(std::vector<std::string>& output)
            {
                output.clear();
                if(!FileUtils::getAllFilesInDirectoryRecursively(this->filePath, output))
                {
                    CLAID_THROW(claid::Exception, "Error in FileReceiverModule, cannot determine missing files.\n"
                    << "Cannot retrieve list of files available in directory \"" << this->filePath << "\".\n"
                    << "Either the directory does not exist or we do not have permissions to access it.");
                }

                for(std::string& path : output)
                {
                    makePathRelative(path);
                }
            }

            void onCompleteFileListReceived(ChannelData<std::vector<std::string>> data)
            {
                const std::vector<std::string>& completeList = data->value();
                Logger::printfln("Received complete file list");

                for(const std::string& value : completeList)
                {
                    Logger::printfln("Complete file %s", value.c_str());
                }                
 
                std::vector<std::string> missingFiles;
                getMissingElements(completeList, missingFiles);

                this->missingFilesQueue = std::queue<std::string>();

                for(const std::string& value : missingFiles)
                {
                    this->missingFilesQueue.push(value);
                    Logger::printfln("Missing %s", value.c_str());
                }
                
                this->requestNextFile();
            }

            void requestNextFile()
            {
                printf("Requesting next file %d\n", this->missingFilesQueue.empty());
                if(this->missingFilesQueue.empty())
                {
                    return;
                }

                std::string file = this->missingFilesQueue.front();
                printf("Requesting file %s\n", file.c_str());
                this->missingFilesQueue.pop();
                this->requestedFileChannel.post(file);
            }

            void onDataFileReceived(ChannelData<DataFile> data)
            {
                std::cout << "Received data file " << data->value().relativePath << "\n";

                this->requestNextFile();
                const DataFile& dataFile = data->value();

                const std::string& relativePath = dataFile.relativePath;
                std::string folderPath;
                std::string filePath;
                splitPathIntoFolderAndFileName(relativePath, folderPath, filePath);
                printf("folder file %s %s\n", folderPath.c_str(), filePath.c_str());
                 
                if(folderPath != "")
                {
                    std::string targetFolderPath = this->filePath + std::string("/") + folderPath;
                    if(!FileUtils::dirExists(targetFolderPath))
                    {
                        if(!FileUtils::createDirectoriesRecursively(targetFolderPath))
                        {
                            CLAID_THROW(Exception, "Error in FileReceiverModule, cannot create target folder \"" << targetFolderPath << "\".");
                        }
                    }
                }

                std::string targetFilePath = 
                    this->filePath + std::string("/") + folderPath + std::string("/") + filePath;
                
                if(!dataFile.saveToPath(targetFilePath))
                {
                    CLAID_THROW(Exception, "Error in FileReceiverModule, cannot create target file \"" << targetFilePath << "\".");
                }

                if(this->storeArrivalTimePerFile)
                {
                    Time arrivalTime = Time::now();
                    std::string arrivalTimeStampPath = 
                        this->filePath + std::string("/") + folderPath + std::string("/") + filePath + std::string("_arrival.xml");
                    
                    XMLSerializer serializer;
                    serializer.serialize(arrivalTime);

                    XMLDocument xmlDocument(serializer.getXMLNode());
                    xmlDocument.saveToFile(arrivalTimeStampPath);
                }

                // Send acknowledgement.
                this->receivedFilesAcknowledgementChannel.post(dataFile.relativePath);
            }

            void setupStorageFolder()
            {
                #ifdef __APPLE__
                    #if TARGET_OS_IPHONE
                        this->filePath = iOSApplicationPathHelper::getAppDocumentsPath() + std::string("/") + this->filePath;
                    #endif
                #endif
                
                std::string savePath = this->filePath;

                if(!FileUtils::dirExists(savePath))
                {
                    if(!FileUtils::createDirectoriesRecursively(savePath))
                    {
                        CLAID_THROW(Exception, "Error in FileReceiverModule. Cannot create save directory \"" << savePath << "\".");
                    }
                }
            }

            void initialize()
            {
                // Create output directory, if not exists.
                this->setupStorageFolder();
                // Check if we can access output directory, scan for existing files.
                // Throws exception if we cannot access the directory.
                std::vector<std::string> output;
                getListOfFilesInTargetDirectory(output);

                this->completeFileListChannel = this->subscribe<std::vector<std::string>>(completeFileListChannelName, &FileReceiverModule::onCompleteFileListReceived, this);
                this->requestedFileChannel = this->publish<std::string>(requestedFileChannelName);
                this->dataFileChannel = this->subscribe<DataFile>(dataFileChannelName, &FileReceiverModule::onDataFileReceived, this);
                this->receivedFilesAcknowledgementChannel = this->publish<std::string>(receivedFilesAcknowledgementChannelName);
            }

        public:

            Reflect(FileReceiverModule,
                reflectMember(filePath);
                reflectMemberWithDefaultValue(storeArrivalTimePerFile, false);

                reflectMemberWithDefaultValue(completeFileListChannelName, std::string("FileSyncer/CompleteFileList"));
                reflectMemberWithDefaultValue(requestedFileChannelName, std::string("FileSyncer/RequestedFileList"));
                reflectMemberWithDefaultValue(dataFileChannelName, std::string("FileSyncer/DataFiles"));
                reflectMemberWithDefaultValue(receivedFilesAcknowledgementChannelName, std::string("FileSyncer/ReceivedFilesAcknowledgement"));
            )

           


    };
}