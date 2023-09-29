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
#include "CLAID.hpp"
#include <fstream>
namespace claid
{
    // Used by DataSyncModule to send files on the filesystem
    // to a DataReceiverModule.
    // Allows to load and save any file from/on the filesystem.
    // All files are loaded in binary mode and not in text mode
    // (but of course, this still works for text files etc.).
    // Binary mode works for all files, while text mode has problems when loading
    // real binary files (e.g., audio data), as some characters might get escaped.
    struct DataFile
    {
        std::string relativePath;
        BinaryData binaryData;


        Reflect(DataFile,
            reflectMember(relativePath);
            reflectMember(binaryData);
        )

        bool loadFromPath(const std::string& path)
        {
            try
            {
                this->binaryData.loadFromFile(path);
                return true;
            }
            catch(std::exception& e)
            {
                return false;
            }
        }

        bool saveToPath(const std::string& path) const
        {
            try
            {
                this->binaryData.saveToFile(path);
                return true;
            }
            catch(std::exception& e)
            {
                return false;
            }
        }

    };
}