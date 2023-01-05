#pragma once

#include <string>
namespace claid
{
    // Helper class that can be used on iOS
    // to retrieve the path to th application's
    // documents directory.
    // See here for more details: https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
    // and here for an interesting blog article about it: https://www.appcoda.com/files-app-integration/
    // tl;dr on iOS we can not save files wherever we want, because each app is sandboxed.
    // For claid, it makes the most sense to write files to the App's Documents directory.
    // The PathHelper can be used to retrieve the path to that directory.
    class iOSApplicationPathHelper
    {
        public:
            static std::string getAppDocumentsPath();
    };
};
