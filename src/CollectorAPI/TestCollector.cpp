#include "CLAID.hpp"

#include "CollectorAPI/Request.hpp"
#include "Audio/AudioData.hpp"

using namespace claid;

class TestCollector : public Module
{
    DECLARE_MODULE(TestCollector)
    private:
        Channel<Request> requestChannel;
        Channel<AudioData> dataChannel;

    
    public:
        void initialize()
        {
            this->requestChannel = this->subscribe<Request>("Requests", &TestCollector::onRequest, this);
            this->dataChannel = this->publish<AudioData>("Test");
        }

        void onRequest(ChannelData<Request> request)
        {
            if(request->value().dataIdentifier == "Test")
            {
                printf("Posting data\n");
                AudioData data;
                dataChannel.post(data);
            }
        }

};
REGISTER_MODULE(TestCollector)