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
