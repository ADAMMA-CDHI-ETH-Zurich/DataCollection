#pragma once


#include "ClassFactory/ClassFactory.hpp"
#include "RunTime/RunTime.hpp"
#include "XML/XMLDeserializer.hpp"

#include "CollectorAPI/CollectorFactory/CollectorFactory.hpp"
#include "CollectorAPI/Collector.hpp"

using namespace portaible::Loader;
namespace portaible
{
	namespace CollectorLoader
	{
		class CollectorLoader : public LoaderBase
		{
			DECLARE_LOADER(CollectorLoader)



			public:
				CollectorLoader() : LoaderBase("Collectors")
				{

				}

				bool execute(std::vector<XMLNode*> xmlNodes) 
				{
					for (XMLNode* node : xmlNodes)
					{
						this->loadCollectors(node);
					}

                    return true;
				}

                private:
                    void loadCollectors(XMLNode* node)     
                    {
                        for(XMLNode* child : node->children)
                        {
                            const std::string& collectorName = child->name;


                            if (!CollectorFactory::CollectorFactory::getInstance()->isFactoryRegisteredForCollector(collectorName))
							{
								PORTAIBLE_THROW(portaible::Exception, "CollectorLoader failed to load Collector from XML. Collector \"" << collectorName << "\" was not registered and is unknown.");
							}


							CollectorAPI::Collector* collector = static_cast<CollectorAPI::Collector*>(CollectorFactory::CollectorFactory::getInstance()->getFactoryForCollectorByName(collectorName)->getInstanceUntyped());
							XMLDeserializer deserializer(node);
							deserializer.deserializeFromNode(collectorName, *collector);

							std::string id;
							if (child->getAttribute("id", id))
							{
								collector->setID(id);
							}


							PORTAIBLE_RUNTIME->addModule(static_cast<Module*>(collector));
                        }
                    }
                    


		};
	}
}

