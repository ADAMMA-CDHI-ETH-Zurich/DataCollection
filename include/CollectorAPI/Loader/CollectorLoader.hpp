#pragma once


#include "ClassFactory/ClassFactory.hpp"
#include "RunTime/RunTime.hpp"
#include "XML/XMLDeserializer.hpp"

#include "CollectorAPI/CollectorFactory/CollectorFactory.hpp"
#include "CollectorAPI/Collector.hpp"

using namespace claid::XMLLoader;
namespace claid
{
	namespace CollectorLoader
	{
		class CollectorLoader : public XMLLoaderBase
		{
			DECLARE_XML_LOADER(CollectorLoader)



			public:
				CollectorLoader() : XMLLoaderBase("Collectors")
				{

				}

				bool execute(std::vector<std::shared_ptr<XMLNode>> xmlNodes) 
				{
					for (std::shared_ptr<XMLNode> node : xmlNodes)
					{
						this->loadCollectors(node);
					}

                    return true;
				}

                private:
                    void loadCollectors(std::shared_ptr<XMLNode> node)     
                    {
                        for(std::shared_ptr<XMLNode> child : node->children)
                        {
                            const std::string& collectorName = child->name;


                            if (!CollectorFactory::CollectorFactory::getInstance()->isFactoryRegisteredForCollector(collectorName))
							{
								CLAID_THROW(claid::Exception, "CollectorLoader failed to load Collector from XML. Collector \"" << collectorName << "\" was not registered and is unknown.");
							}


							CollectorAPI::Collector* collector = static_cast<CollectorAPI::Collector*>(CollectorFactory::CollectorFactory::getInstance()->getFactoryForCollectorByName(collectorName)->getInstanceUntyped());
							XMLDeserializer deserializer(node);
							deserializer.deserializeFromNode(collectorName, *collector);

							std::string id;
							if (child->getAttribute("id", id))
							{
								collector->setID(id);
							}


							CLAID_RUNTIME->addModule(static_cast<Module*>(collector));
                        }
                    }
                    


		};
	}
}

