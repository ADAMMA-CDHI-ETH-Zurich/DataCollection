#pragma once
#include "CLAID.hpp"
#include <fstream>
namespace claid
{
    // Used by FileSyncerModule to send files on the filesystem
    // to a FileReceiverModule.
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