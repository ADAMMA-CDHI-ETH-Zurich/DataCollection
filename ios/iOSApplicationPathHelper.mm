#include "CollectorAPI/iOSHelper/iOSApplicationPathHelper.hpp"

namespace claid
{
    std::string iOSApplicationPathHelper::getAppDocumentsPath()
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        std::string stdDocumentsPath([documentsDirectory UTF8String]);

        return stdDocumentsPath;
    }

}
