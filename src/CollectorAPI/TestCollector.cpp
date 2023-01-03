#include "CLAID.hpp"

#include "CollectorAPI/Request.hpp"
#include "Audio/AudioData.hpp"

using namespace claid;

class TestCollector : public Module
{
    private:
        Channel<Request> requestChannel;
        Channel<AudioData> dataChannel;

    
    public:
        void initialize()
        {
            this->requestChannel = this->subscribe<Request>("Requests", &TestCollector::onRequest, this);
            this->dataChannel = this->publish<AudioData>("AudioData");
        }

        void onRequest(ChannelData<Request> request)
        {
            if(request->value().dataIdentifier == "Test")
            {
                AudioData data;
                dataChannel.post(data);
            }
        }

};