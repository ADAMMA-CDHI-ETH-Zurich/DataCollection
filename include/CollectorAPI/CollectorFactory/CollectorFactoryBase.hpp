#pragma once

namespace portaible
{
	namespace CollectorFactory
	{
		class CollectorFactoryBase
		{
			public:
				virtual void* getInstanceUntyped() = 0;
		};
	}
}