#include "CollectorAPI/Request.hpp"

namespace portaible
{
    Request::Request()
    {
        this->requestDescription = std::shared_ptr<RequestDescription>(new RequestDescription); 
    }

    Request::Request(std::string dataIdentifier) : dataIdentifier(dataIdentifier)
    {
        this->requestDescription = std::shared_ptr<RequestDescription>(new RequestDescription); 

    }


    Request::Request(std::string dataIdentifier, std::shared_ptr<RequestDescription> requestDescription) : requestDescription(requestDescription)
    {

    }


    const Time Request::getNextDueTime()
    {
        // Change later for saved values;
        Time lastExecutionTime = Time::currentDay();
        bool hasEverBeenExecuted = true;

        int hour, minute, second;
        requestDescription->getDueHourMinuteSecond(hour, minute, second);

       
        
        if(!hasEverBeenExecuted)
        {
            // Schedule execution today at specified time.

            // TODO: What if todayAt is already past ? 
            return Time::todayAt(hour, minute, second);
        }

        if(this->requestDescription->repetitionDecription.type == RepetitionDescription::Type::NEVER)
        {
            return Time::unixEpoch();
        }

        return calculateNextExecutionTime(lastExecutionTime);
        


    }

    Time Request::calculateNextExecutionTime(const Time& lastExecutionTime)
    {
        Time now = Time::now();
        Time nextExecutionTime = lastExecutionTime;

        int secondsToIncrease = this->requestDescription->repetitionDecription.hours * 60 * 60 +
                                    this->requestDescription->repetitionDecription.minutes * 60 +
                                        this->requestDescription->repetitionDecription.seconds;

        // Calculate next execution.
        while (nextExecutionTime < now)
        {
            nextExecutionTime += std::chrono::seconds(secondsToIncrease);
        } 

        return nextExecutionTime;
    }

}