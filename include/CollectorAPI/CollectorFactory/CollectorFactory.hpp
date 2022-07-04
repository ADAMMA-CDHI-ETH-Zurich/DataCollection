#pragma once


#include "CollectorFactoryBase.hpp"
#include "CollectorFactoryTyped.hpp"

#include "Exception/Exception.hpp"
#include "Utilities/Singleton.hpp"

#include <map>
#include <string>
#include <vector>

namespace portaible
{
	namespace CollectorFactory
	{
		class CollectorFactory : public Singleton<CollectorFactory>
		{
			private:
				std::map<std::string, CollectorFactoryBase*> collectorFactories;

			public:
				template<typename T>
				void registerFactory(const std::string& className)
				{
					auto it = collectorFactories.find(className);

					if (it != collectorFactories.end())
					{
						PORTAIBLE_THROW(portaible::Exception, "Error, class \"" << className << "\" was registered to CollectorFactory more than once");
					}

					collectorFactories.insert(std::make_pair(className, static_cast<CollectorFactoryBase*>(new CollectorFactoryTyped<T>)));
				}

				bool isFactoryRegisteredForCollector(const std::string& collectorName)
				{
					auto it = collectorFactories.find(collectorName);

					return it != collectorFactories.end();
				}

				CollectorFactoryBase* getFactoryForCollectorByName(const std::string& collectorName)
				{
					if (!isFactoryRegisteredForCollector(collectorName))
					{
						return nullptr;
					}

					auto it = collectorFactories.find(collectorName);
					return it->second;
				}

				template<typename T>
				T* getNewInstanceAndCast(const std::string& collectorName)
				{
					if (!isFactoryRegisteredForCollector(collectorName))
					{
						return nullptr;
					}
					
					T* obj = static_cast<T*>(getFactoryForCollectorByName(collectorName)->getInstanceUntyped());
					return obj;
				}

				std::vector<std::string> getRegisteredCollectorNames()
				{
					std::vector<std::string> output;

					for(auto it : this->collectorFactories)
					{
						output.push_back(it.first);
					}

					return output;
				}
		};

		template<typename T>
		class RegisterHelper
		{
			public:
				RegisterHelper(std::string name) 
				{
					CollectorFactory::getInstance()->registerFactory<T>(name);
				}	

				static std::string getCollectorNameFromClassName(std::string className)
				{
					// If className is something like namespace::Collector, remove namespace:: and return Collector.
					// Otherwise, if className is Collector (e.g. MicrophoneDataCollector), just return Collector.

					size_t index = className.find_last_of("::");
					if(index == std::string::npos)
					{
						return className;
					}

					// Return everything after the last occurence of ::
					return className.substr(index + 1, className.size());

				}
		};

	}
}


#define DECLARE_COLLECTOR_FACTORY(className) \
	static volatile portaible::CollectorFactory::RegisterHelper<className> collectorFactoryRegistrar;\
	static const std::string __COLLECTOR_NAME__;\
	const std::string& getCollectorName() {return className::__COLLECTOR_NAME__;}

#define REGISTER_TO_COLLECTOR_FACTORY(className) \
	volatile portaible::CollectorFactory::RegisterHelper<className> className::collectorFactoryRegistrar (portaible::CollectorFactory::RegisterHelper<className>::getCollectorNameFromClassName(#className));\
	const std::string className::__COLLECTOR_NAME__ = portaible::CollectorFactory::RegisterHelper<className>::getCollectorNameFromClassName(#className);
