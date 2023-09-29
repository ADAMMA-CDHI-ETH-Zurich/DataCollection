/***************************************************************************
* Copyright (C) 2023 ETH Zurich
* Core AI & Digital Biomarker, Acoustic and Inflammatory Biomarkers (ADAMMA)
* Centre for Digital Health Interventions (c4dhi.org)
* 
* Authors: Patrick Langer, Stephan Altmüller, Francesco Feher
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*         http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
***************************************************************************/

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

