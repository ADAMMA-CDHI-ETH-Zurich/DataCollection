#pragma once

#include "CollectorFactoryBase.hpp"

namespace portaible
{
	namespace CollectorFactory
	{
		template<typename T>
		class CollectorFactoryTyped : public CollectorFactoryBase
		{
			public:
				void* getInstanceUntyped()
				{
					return static_cast<void*>(new T);
				}

				T* getInstance()
				{
					return new T;
				}
		};
	}
}