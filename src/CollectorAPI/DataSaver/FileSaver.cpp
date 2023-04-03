#include "CollectorAPI/DataSaver/FileSaver.hpp"
#include "Utilities/FileUtils.hpp"
#include "CollectorAPI/DataSaver/DataSaverModule.hpp"
namespace claid
{
    void FileSaver::initialize(DataSaverModule* parentModule)
    {  
        this->parentModule = parentModule;
        this->dataChannel = this->parentModule->subscribe<Untyped>(this->what, &FileSaver::onData, this);

        this->createStorageFolder(Path(""));
        this->createTmpFolderIfRequired(Path(""));
    }

    void FileSaver::onData(ChannelData<Untyped> data)
    {
        std::string dataTypeName = this->dataChannel.getChannelDataTypeName();
        std::string reflectorName = this->serializer->getReflectorName();

        data.applySerializerToData(this->serializer, !this->excludeHeader);

        std::vector<char> writeableData;
        this->serializer.get()->getDataWriteableToFile(writeableData);

        Path path;
        this->getCurrentPathRelativeToStorageFolder(path, data.getTimestamp());
        storeData(writeableData, data);
    }

    void FileSaver::storeData(std::vector<char>& data, ChannelData<Untyped>& d)
    {
        std::string pathStr = d.getHeader().timestamp.strftime(this->fileNameFormat.c_str());
        Path path(pathStr);

        Path oldPath = path;
        Path oldCurrentPath = this->currentPath;
        if(path != this->currentPath)
        {
            this->currentPath = path;
            beginNewFile(this->currentPath);
        }

        this->file.write(data.data(), data.size());

        // this->file << "," <<  d.getTimestamp().toUnixTimestampMilliseconds() << "," << d.getHeader().timestamp.strftime(this->fileNameFormat.c_str()) << "," << pathStr;
        // this->file << "," <<  oldPath.toString() << "," << oldCurrentPath.toString() << "," << path.toString() << "," << currentPath.toString() << "," ;

        this->file << "\n";

    }

    void FileSaver::beginNewFile(const Path& path)
    {
        this->file.flush();
        this->file.close();

        this->createStorageFolder(path);
        this->createTmpFolderIfRequired(path);

        if(this->tmpStoragePath != "")
        {
            this->moveTemporaryFilesToStorageDestination();
            this->openFile(Path::join(this->tmpStoragePath, path).toString());
        }
        else
        {
            this->openFile(Path::join(this->storagePath, path).toString());
        }

        std::vector<char> headerData;
        this->serializer->getHeaderWriteableToFile(headerData);
        this->storeDataHeader(headerData);
    }

    void FileSaver::storeDataHeader(std::vector<char>& dataHeader)
    {
        if(dataHeader.size() == 0)
        {
            return;
        }

        this->file.write(dataHeader.data(), dataHeader.size());
        this->file << "\n";
    }

    void FileSaver::getCurrentPathRelativeToStorageFolder(Path& path, const Time timestamp)
    {
        Time time = timestamp;
        path = Path(time.strftime(this->fileNameFormat.c_str()));
        //path = Path::join(this->storagePath, path.toString());
    }
     
    void FileSaver::createDirectoriesRecursively(const std::string& path)
    {
        if(FileUtils::dirExists(path))
        {
            return;
        }
        if(!FileUtils::createDirectoriesRecursively(path))
        {
            CLAID_THROW(Exception, "Error in FileSaver of DataSaverModule: Failed to create one or more directories of the following path: " << path);
        }
    }

    void FileSaver::openFile(const std::string& path)
    {
        this->file = std::ofstream(path, std::ios::app | std::ios::binary);

        if(!this->file.is_open())
        {
            CLAID_THROW(Exception, "Error in FileSaver of DataSaverModule: Failed to open file \"" << path << "\" for writing.");
        }
    }

    void FileSaver::createStorageFolder(const Path& currentSavePath)
    {
        std::string folderPath = Path::join(this->storagePath, currentSavePath.getFolderPath()).toString();
        this->createDirectoriesRecursively(folderPath);
    }

    void FileSaver::createTmpFolderIfRequired(const Path& currentSavePath)
    {
        if(this->tmpStoragePath == "")
        {
            return;
        }
        std::string folderPath = Path::join(this->tmpStoragePath, currentSavePath.getFolderPath()).toString();
        this->createDirectoriesRecursively(folderPath);
    }

    // In some cases, it might be benefical to first store files in a temporary location,
    // and only moving them to the final storage destination when writing to the file has been finished.
    // The user can choose to do that by specifying tmpFolderPath.
    // Whenever a new file is opened, all files in the tmp folder will be moved to their actual destination.
    void FileSaver::moveTemporaryFilesToStorageDestination()
    {
        std::vector<std::string> files;
        FileUtils::getAllFilesInDirectoryRecursively(this->tmpStoragePath, files);

        Logger::printfln("querying tmp folder %s\n", tmpStoragePath.c_str());
        for(const std::string& filePath : files)
        {
            Path path(filePath);
            
            std::string relativePath = path.getPathRelativeTo(this->tmpStoragePath);

            // Don't move the currently active file.
            Logger::printfln("Current path : %s Relative path: %s", currentPath.toString().c_str(), relativePath.c_str());
            if(relativePath == this->currentPath.toString())
            {
                Logger::printfln("Current path is relative path, not moving.");
                continue;
            }
            // If a file already exists at storage path (which it shoudln't!), then it will not be overriden but appended.
            if(!FileUtils::moveFileTo(Path::join(this->tmpStoragePath, relativePath), Path::join(this->storagePath, relativePath), true))
            {
                Logger::printfln("Moving file %s to %s failed.", Path::join(this->tmpStoragePath, relativePath).toString().c_str(), Path::join(this->storagePath, relativePath).toString().c_str());
            }
        }
    }
}