#include "CollectorAPI/DataSaver/DataSaverModule.hpp"

namespace claid
{
    void DataSaverModule::initialize()
    {
        for(FileSaver& fileSaver : this->fileSavers)
        {
            fileSaver.initialize(this);
        }
    }
}

REGISTER_MODULE(claid::DataSaverModule)

