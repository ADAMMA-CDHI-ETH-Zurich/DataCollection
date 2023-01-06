#include "CollectorAPI/RequestHandler.hpp"
#include "CollectorAPI/RequestModule.hpp"
#include "XML/XMLDocument.hpp"


#ifdef __APPLE__
    #if TARGET_OS_IPHONE
    // Changes this->filePath from e.g., Measurements/AudioData to 
    // /var/mobile/Containers/Data/Application/{iOS_APP_ID}/Documents/Measurements/AudioData
    // (path and iOS_APP_ID are determined automatically using iOSApplicationPathHelper).
    #include "CollectorAPI/iOSHelper/iOSApplicationPathHelper.hpp"
    #endif
#endif

namespace claid
{
    RequestHandler::RequestHandler(const int handlerID) : handlerID(handlerID)
    {

    }

    RequestHandler::RequestHandler(RequestModule* parent, 
                    const RequestDescription& requestDescription, 
                    const int handlerID) : handlerID(handlerID), parent(parent), requestDescription(requestDescription)
    {
        
    }

    std::string RequestHandler::getCurrentRecordingPath() const
    {
        return this->requestDescription.saveTo + std::string("/") + std::to_string(this->existingRecordingsInDirectory);
    }

    std::string RequestHandler::getPeriodicFunctionName() const
    {
        // Append handlerID, because it could happen that we have multiple RequestHandlers for the
        // same data identifier.
        return this->requestDescription.what + std::string("_") + std::to_string(this->handlerID);
    }

    void RequestHandler::setupStorageFolder()
    {
        if(this->requestDescription.saveTo == "")
        {
            return;
        }
        
        std::string savePath = this->requestDescription.saveTo;

        if(!FileUtils::dirExists(savePath))
        {
            if(!FileUtils::createDirectoriesRecursively(savePath))
            {
                CLAID_THROW(Exception, "Error in RequestHandler. Cannot create save directory \"" << savePath << "\".");
            }
        }

        std::vector<std::string> existingDirectories;
        FileUtils::getAllDirectoriesInDirectory(savePath, existingDirectories);
        this->existingRecordingsInDirectory = existingDirectories.size();

        if(!FileUtils::createDirectoriesRecursively(this->getCurrentRecordingPath()))
        {
            CLAID_THROW(Exception, "Error in RequestHandler. Cannot create save directory \"" << this->getCurrentRecordingPath() << "\".");
        }
    }

    void RequestHandler::setupRequest()
    {
        std::string dataIdentifier = this->requestDescription.what;
        this->dataChannel = this->parent->subscribe<Untyped>(dataIdentifier, &RequestHandler::onData, this);

        Logger::printfln("Register function %d\n", this->requestDescription.period);
        this->parent->registerPeriodicFunction(dataIdentifier, &RequestHandler::periodicRequest, this, this->requestDescription.period);
    }

    void RequestHandler::onData(ChannelData<Untyped> data)
    {
        Logger::printfln("on data %s\n", this->requestDescription.saveTo.c_str());
        if(requestDescription.saveTo == "")
        {
            return;
        }

        std::string path = this->getCurrentRecordingPath() + std::string("/") + std::to_string(this->numSamples) + std::string(".xml");

        if(this->requestDescription.format == "Binary")
        {
            std::string path = this->getCurrentRecordingPath() + std::string("/") + std::to_string(this->numSamples) + std::string(".raw");
            TaggedData<BinaryData> binaryData = data.getBinaryData();
            binaryData.value().saveToFile(path);
        }
        else
        {
            std::string path = this->getCurrentRecordingPath() + std::string("/") + std::to_string(this->numSamples) + std::string(".xml");
            
            if(data.canSerializeToXML())
            {
                XMLDocument document;
                document.setXMLNode(data.toXML());
                document.saveToFile(path);  
            }
            else
            { 
                CLAID_THROW(claid::Exception, "Cannot serialize requested data \"" << this->requestDescription.what << "\" to XML. "
                << "We do not know the type of the data, as no other publisher/subscriber is available for the channel ID in this instance of CLAID (this process).\n"
                << "If you try to record data coming from a remote connection, you need to manually create a typed publisher/subscriber for this channel\n"
                << "(e.g., start a Module that publishes to the Channel with your desired type.");
            }

            
        }
        
        std::string headerPath = this->getCurrentRecordingPath() + std::string("/stamp_") + std::to_string(this->numSamples) + std::string(".xml");
        XMLDocument headerDocument;
        headerDocument.setXMLNode(data.headerToXML());
        headerDocument.saveToFile(headerPath);  
        this->numSamples++;

    }

    #ifdef __APPLE__
        #if TARGET_OS_IPHONE
        void RequestHandler::prependiOSDocumentsPathToFilePath()
        {
            this->requestDescription.saveTo = iOSApplicationPathHelper::getAppDocumentsPath() + std::string("/") + this->requestDescription.saveTo;
        }
        #endif
    #endif

    void RequestHandler::periodicRequest()
    {
        Logger::printfln("Sending request %s\n", this->requestDescription.what.c_str());
        Request request;
        request.dataIdentifier = this->requestDescription.what;
        this->parent->postRequest(request);
    }

    void RequestHandler::initialize()
    {
        #ifdef __APPLE__
            #if TARGET_OS_IPHONE
            this->prependiOSDocumentsPathToFilePath();
            #endif
        #endif
        setupStorageFolder();
        setupRequest();
    }

    void RequestHandler::terminate()
    {
        this->dataChannel.unsubscribe();
        this->parent->unregisterPeriodicFunction(this->requestDescription.what);
    }
}
