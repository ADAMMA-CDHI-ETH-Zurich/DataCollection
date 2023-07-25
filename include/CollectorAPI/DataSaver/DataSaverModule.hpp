#pragma once
#include "CLAID.hpp"
#include "Serialization/Serializer.hpp"
#include <fstream>
#include "CollectorAPI/DataSaver/FileSaver.hpp"
#include "StandardReflectors/ListReflector.hpp"
namespace claid
{
    class DataSaverModule : public claid::Module
    {
        friend class FileSaver;

        private:
            std::vector<FileSaver> fileSavers;


        public:
            void initialize();
            

            Reflect(DataSaverModule, 
                ListReflector<std::vector<FileSaver>>("save", this->fileSavers).reflect(reflector);
                
            )
    };
}