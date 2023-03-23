#include "CollectorAPI/DataSaverModule.hpp"

namespace claid
{
    void DataSaverModule::initialize()
    {
        this->dataChannel = this->subscribe<Untyped>(this->dataChannelName, &DataSaverModule::onData, this);
    }

    void DataSaverModule::onData(ChannelData<Untyped> data)
    {
        printf("Received data in channel, data type name is %s\n", this->dataChannel.getChannelDataTypeName().c_str());   
        std::string dataTypeName = this->dataChannel.getChannelDataTypeName();
        std::string reflectorName = this->serializer->getReflectorName();

        data.applySerializerToData(serializer);

        std::vector<char> writeableData;
        printf("get byte rep\n");
        serializer.get()->getByteRepresentationOfSerializedData(writeableData);
        storeData(writeableData);
    }

    void DataSaverModule::storeData(std::vector<char>& data)
    {
        std::string bla(data.data(),data.size());
        printf("data %s\n", bla.c_str());
    }

}

REGISTER_MODULE(claid::DataSaverModule)

