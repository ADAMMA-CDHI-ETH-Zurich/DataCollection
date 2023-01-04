#pragma once
#include "CLAID.hpp"
#include "Utilities/FileUtils.hpp"
#include "Utilities/StringUtils.hpp"
#include "DataFile.hpp"


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

            // On this channel, we post the list of files
            // that are missing.
            // This will be a subset of the list send in the completeFileListChannel.
            Channel<std::vector<std::string>> requestedFileListChannel;

            // On this channel, we receive files from the FileSyncerModule.
            Channel<DataFile> dataFileChannel;

            std::string filePath;
            std::string completeFileListChannelName;
            std::string requestedFileListChannelName;
            std::string dataFileChannelName;


            template<typename T>
            bool isElementContainedInVector(const std::vector<T>& vector, const T& element)
            {
                return std::find(vector.begin(), vector.end(), element) != vector.end();
            }

            // Get elements that are contained in both lists.
            void getMissingElements(
                    const std::vector<std::string>& completeList, 
                    const std::vector<std::string>& localList,
                    std::vector<std::string>& missingList)
            {
                missingList.clear();
                for(const std::string& value : completeList)
                {
                    if(!this->isElementContainedInVector(localList, value))
                    {
                        missingList.push_back(value);
                    }
                }
            }

            void makePathRelative(std::string& path)
            {
                if(StringUtils::startsWith(path, this->filePath))
                {
                    path = path.substr(this->filePath.size(), path.size());   
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
                }

                int index = path.size() - 1;
                while(index > 0)
                {
                    if(path[index] == '/')
                    {
                        folderPath = path.substr(0, index - 1);
                        fileName = path.substr(index + 1, path.size());
                        return;
                    }
                    index --;
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
                // Find the files we are missing in the folder.
                std::vector<std::string> localList;
                getListOfFilesInTargetDirectory(localList);

                const std::vector<std::string>& completeList = data->value();

                for(const std::string& value : completeList)
                {
                    std::cout << value << "\n";
                }

                std::vector<std::string> missingFiles;
                getMissingElements(completeList, localList, missingFiles);

                // Send the list of missing files to the FileSyncerModule.
                // Afterwards, it will send us the missing files.
                this->requestedFileListChannel.post(missingFiles);
            }

            void onDataFileReceived(ChannelData<DataFile> data)
            {
                std::cout << "Received data file " << data->value().relativePath << "\n";

                const DataFile& dataFile = data->value();

                const std::string& relativePath = dataFile.relativePath;
                std::string folderPath;
                std::string filePath;
                splitPathIntoFolderAndFileName(relativePath, folderPath, filePath);

                 
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
            }

            void setupStorageFolder()
            {
                if(this->filePath == "")
                {
                    return;
                }
                
                std::string savePath = this->filePath;

                if(!FileUtils::dirExists(savePath))
                {
                    if(!FileUtils::createDirectoriesRecursively(savePath))
                    {
                        CLAID_THROW(Exception, "Error in FileReceiverModule. Cannot create save directory \"" << savePath << "\".");
                    }
                }
            }

        public:

            Reflect(FileReceiverModule,
                reflectMember(filePath);
                reflectMemberWithDefaultValue(completeFileListChannelName, std::string("FileSyncer/CompleteFileList"));
                reflectMemberWithDefaultValue(requestedFileListChannelName, std::string("FileSyncer/RequestedFileList"));
                reflectMemberWithDefaultValue(dataFileChannelName, std::string("FileSyncer/DataFiles"));
            )

            void initialize()
            {
                // Create output directory, if not exists.
                this->setupStorageFolder();
                // Check if we can access output directory, scan for existing files.
                // Throws exception if we cannot access the directory.
                std::vector<std::string> output;
                getListOfFilesInTargetDirectory(output);

                this->completeFileListChannel = this->subscribe<std::vector<std::string>>(completeFileListChannelName, &FileReceiverModule::onCompleteFileListReceived, this);
                this->requestedFileListChannel = this->publish<std::vector<std::string>>(requestedFileListChannelName);
                this->dataFileChannel = this->subscribe<DataFile>(dataFileChannelName, &FileReceiverModule::onDataFileReceived, this);
            }


    };
}