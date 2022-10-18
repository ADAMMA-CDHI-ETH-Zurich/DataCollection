#pragma once

#include <vector>

namespace claid
{
    template<typename T>
    class PolymorphicClassList
    {


        public:

        // Implement custom vector reflect..
        template<typename Reflector>
        void reflect(Reflector& r)
        {
            for(size_t i = 0; i < data.size(); i++)
            {
                T*& element = this->data[i];
                r.member(element->getClassName(), element, "");
            }
        }


        private:
            std::vector<T*> data;

    };
}