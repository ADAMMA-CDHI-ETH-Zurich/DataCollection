#include "CollectorAPI/RequestDescription.hpp"
#include "Exception/Exception.hpp"

#include <stdlib.h>
#include <ctype.h>
#include <cmath>

namespace portaible
{
    void RequestDescription::getDueHourMinuteSecond(int& hour, int& minute, int& second)
    {
        hour = 0;
        minute = 0;
        second = 0;

        int index = this->atTime.size() - 1;

        // We only have hour, minute and second
        const int numReferences = 3;
        int* references[numReferences] = {&second, &minute, &hour};
        
        int referenceIndex = 0;

        int* current = references[0];

        // "LeetCode ;)"
        int digit = 0;
        int shift = 1;
        while(index >= 0)
        {
            const char& currentChar = this->atTime[index];
            if(std::isdigit(currentChar))
            {
                digit = static_cast<int>(currentChar) - static_cast<int>('0');
                *current = (*current) + digit * shift;
                shift *= 10;
                // Overflow handling would be:
                // if(current > INT_MAX/10 || (current == INT_MAX/10 && digit > 7))
            }
            else if(currentChar == ':')
            {
                referenceIndex++;
                                
                if(referenceIndex >= numReferences)
                {
                    PORTAIBLE_THROW(Exception, "Error, too many ':' in time string atTime. Only hour:minute:second is allowed.");
                }
                else
                {
                    current = references[referenceIndex];
                    shift = 1;
                }
            }
            else
            {
                PORTAIBLE_THROW(Exception, "Error, unrecognized character in string atTime: " << currentChar);
            }
            index--;
        }
        verify(hour, minute, second);

    }

    void RequestDescription::verify(const int& hour, const int& minute, const int& second)
    {
        if(hour > 23 || hour < 0)
        {
            PORTAIBLE_THROW(Exception, "Error: Failed to parse string atTime: Hour outside range [0, 23]");
        }

        if(minute > 59 || minute < 0)
        {
            PORTAIBLE_THROW(Exception, "Error: Failed to parse string atTime: Minute outside range [0, 59]");
        }

        if(second > 59 || second < 0)
        {
            PORTAIBLE_THROW(Exception, "Error: Failed to parse string atTime: Second outside range [0, 59]");
        }
    }

}

REGISTER_SERIALIZATION(portaible::RequestDescription)