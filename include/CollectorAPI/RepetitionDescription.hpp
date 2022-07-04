#pragma once

namespace portaible
{
    struct RepetitionDescription
    {
        /*  Specifies when to repeat a data request after the previous request.
            E.g. 2.5hrs after the last request finished 
                For Collectors that record data over time, this means the request is repeated 2.5 hrs AFTER the recording has FINISHED.
            
            Or: 20 minutes after the last request was issued.
                For Collectors that record data over time, this means the request is repeated 2.5hrs after the recording has STARTED.
        */

       enum Type
       {
           AFTER_FINISHED,
           AFTER_START,
           NEVER
       };

        int seconds;
        int minutes;
        int hours;
        Type type;
    
    };
}