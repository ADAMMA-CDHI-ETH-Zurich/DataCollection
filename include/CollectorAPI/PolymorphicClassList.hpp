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