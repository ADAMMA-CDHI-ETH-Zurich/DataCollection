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

    void RequestHandler::storeBinaryDataSample(TaggedData<BinaryData> binaryData, const std::string& suffix)
    {
        std::string path = this->getCurrentRecordingPath() + std::string("/") + std::to_string(this->numSamples) + std::string(suffix);
        binaryData.value().saveToFile(path);
    }
        
    void RequestHandler::storeXMLDataSample(std::shared_ptr<XMLNode> xml, const std::string& suffix)
    {
        std::string path = this->getCurrentRecordingPath() + std::string("/") + std::to_string(this->numSamples) + std::string(suffix);
        XMLDocument document;
        document.setXMLNode(xml);
        document.saveToFile(path);  
    }

    void RequestHandler::updateDataBundle()
    {
        if(this->dataBuffer.size() < this->requestDescription.bundleNSamplesIntoOne)
        {
            return;
        }

        if(requestDescription.format == "Binary")
        {
            BinaryData binaryData;
            for(ChannelData<Untyped> data : this->dataBuffer)
            {
                binaryData.append(data.getBinaryData().value());
            }
            storeBinaryDataSample(binaryData, ".claid_binary");
        }
        else if(requestDescription.format == "XML")
        {
            std::vector<std::shared_ptr<XMLNode>> nodes;
            for(ChannelData<Untyped> data : this->dataBuffer)
            {
                if(data.canSerializeToXML())
                {
                    std::shared_ptr<XMLNode> xmlNode = data.toXML();
                    nodes.push_back(xmlNode);       
                }
                else
                { 
                    CLAID_THROW(claid::Exception, "Cannot serialize requested data \"" << this->requestDescription.what << "\" to XML. "
                    << "We do not know the type of the data, as no other publisher/subscriber is available for the channel ID in this instance of CLAID (this process).\n"
                    << "If you try to record data coming from a remote connection, you need to manually create a typed publisher/subscriber for this channel\n"
                    << "(e.g., start a Module that publishes to the Channel with your desired type.");
                }

            }
            std::shared_ptr<XMLNode> mergedNode = XMLNode::mergeAsCollection(nodes, "item");
            storeXMLDataSample(mergedNode, "_merged.xml");
        }

        std::vector<std::shared_ptr<XMLNode>> stampNodes;
        for(ChannelData<Untyped> data : this->dataBuffer)
        {
            stampNodes.push_back(data.headerToXML());
        }
        std::shared_ptr<XMLNode> mergedStampNodes = XMLNode::mergeAsCollection(stampNodes, "item");
        this->storeXMLDataSample(mergedStampNodes, "_stamp.xml");

        this->numSamples++;
        this->dataBuffer.clear();
    }

    void RequestHandler::onData(ChannelData<Untyped> data)
    {
        Logger::printfln("on data %s\n", this->requestDescription.saveTo.c_str());
        if(requestDescription.saveTo == "")
        {
            return;
        }

        if(requestDescription.bundleNSamplesIntoOne > 0)
        {
            this->dataBuffer.push_back(data);
            this->updateDataBundle();
            return;
        }


        if(this->requestDescription.format == "Binary")
        {
            TaggedData<BinaryData> binaryData = data.getBinaryData();
            this->storeBinaryDataSample(binaryData, ".claid_binary");
        }
        else if(this->requestDescription.format == "XML")
        {            
            if(data.canSerializeToXML())
            {
                std::shared_ptr<XMLNode> xmlNode = data.toXML();
                storeXMLDataSample(xmlNode, ".xml");       
            }
            else
            { 
                CLAID_THROW(claid::Exception, "Cannot serialize requested data \"" << this->requestDescription.what << "\" to XML. "
                << "We do not know the type of the data, as no other publisher/subscriber is available for the channel ID in this instance of CLAID (this process).\n"
                << "If you try to record data coming from a remote connection, you need to manually create a typed publisher/subscriber for this channel\n"
                << "(e.g., start a Module that publishes to the Channel with your desired type.");
            }
        }
        else
        {
            CLAID_THROW(Exception, "Error in RequestHandler of RequestModule: Cannot store data in format \"" << this->requestDescription.format << "\".\n"
            << "The format is unknown and not supported. Supported formats are : [XML, Binary]");
        }

        storeXMLDataSample(data.headerToXML(), "_stamp.xml");
       
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
