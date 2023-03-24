#include "CollectorAPI/DataSaver/FileSaver.hpp"
#include "Utilities/FileUtils.hpp"
#include "CollectorAPI/DataSaver/DataSaverModule.hpp"
namespace claid
{
    void FileSaver::initialize(DataSaverModule* parentModule)
    {  
        this->parentModule = parentModule;
        this->dataChannel = this->parentModule->subscribe<Untyped>(this->what, &FileSaver::onData, this);
    }

    void FileSaver::onData(ChannelData<Untyped> data)
    {
        std::string dataTypeName = this->dataChannel.getChannelDataTypeName();
        std::string reflectorName = this->serializer->getReflectorName();

        this->serializer->forceReset();
        data.applySerializerToData(this->serializer, true);

        std::vector<char> writeableData;
        this->serializer.get()->getDataWriteableToFile(writeableData);
        storeData(writeableData);
    }

    void FileSaver::storeData(std::vector<char>& data)
    {
        Path path;
        this->getCurrentPath(path);
    
        if(path != this->lastPath)
        {
            this->file.close();
            std::string folderPath = path.getFolderPath();
            this->createDirectoriesRecursively(folderPath);

            this->openFile(path.toString());
        }

        this->file.write(data.data(), data.size());
    }

    void FileSaver::getCurrentPath(Path& path)
    {
        Time time = Time::now();
        path = Path(time.strftime(this->storagePathFormat.c_str()));
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
}