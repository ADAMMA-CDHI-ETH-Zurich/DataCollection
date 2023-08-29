#pragma once

#include "CLAID.hpp"
#include "Reflection/SplitReflect.hpp"
#include "Utilities/StringUtils.hpp"

namespace claid
{
    class PeriodicValue
    {
        private:
            double periodInSeconds; // 1.1 is 1 second 100 ms
            bool wasSpecifiedAsFrequency = false;

            void parseFromString(const std::string& string)
            {
                if(string == "")
                {
                    this->periodInSeconds = 0.0;
                    return;
                }

                std::string filteredString = string;
                StringUtils::stringReplaceAll(filteredString, " ", "");
                StringUtils::stringReplaceAll(filteredString, "\t", "");
                StringUtils::stringReplaceAll(filteredString, "\n", "");

                
                std::string value = "";
                std::string unit = "";
                size_t index = 0;


                while(index < filteredString.size() && std::isdigit(filteredString[index]))
                {
                    value += filteredString[index];
                    index++;
                }

                while(index < filteredString.size())
                {
                    unit += filteredString[index];
                    index++;
                }

                try
                {
                    this->periodInSeconds = stod(value);
                }
                catch(std::exception& e)
                {
                    CLAID_THROW(claid::Exception, "Failed to parse periodic value.\n"
                    << "Unable to convert string \"" << value << "\" to a floating point value.\n"
                    << "Error was: \"" << e.what() << "\".");
                }

                this->wasSpecifiedAsFrequency = false;

                if(unit == "")
                {
                    // Then we interprete it as seconds. 
                    // Nothing to do, since we already stored value in periodInSeconds above.
                }
                else if(unit == "GHz")
                {
                    this->periodInSeconds = 1/(this->periodInSeconds * 1000000000);
                    this->wasSpecifiedAsFrequency = true;
                }
                else if(unit == "MHz")
                {
                    this->periodInSeconds = 1/(this->periodInSeconds * 1000000);
                    this->wasSpecifiedAsFrequency = true;
                }
                else if(unit == "kHz")
                {
                    this->periodInSeconds = 1/(this->periodInSeconds * 1000);
                    this->wasSpecifiedAsFrequency = true;
                }
                else if(unit == "Hz")
                {
                    this->periodInSeconds = 1/(this->periodInSeconds);
                    this->wasSpecifiedAsFrequency = true;
                }
                else if(unit == "mHz")
                {
                    this->periodInSeconds = 1/(this->periodInSeconds * 1.0/1000.0);
                    this->wasSpecifiedAsFrequency = true;
                }
                
                
                
                else if(unit == "h") // hour
                {
                    this->periodInSeconds = this->periodInSeconds * 60 * 60;
                }
                else if(unit == "m") // minute
                {
                    this->periodInSeconds = this->periodInSeconds * 60;
                }
                else if(unit == "s") // second
                {
                    // this->periodInSeconds = this->periodInSeconds;
                }
                else if(unit == "ms") // milliseconds
                {
                    this->periodInSeconds = this->periodInSeconds / 1000;    
                }
                else if(unit == "mus") // microseconds
                {
                    this->periodInSeconds = this->periodInSeconds / 1000000;    
                }
                else if(unit == "ns") // nanoseconds
                {
                    this->periodInSeconds = this->periodInSeconds / 1000000000;    
                }
                else
                {
                    CLAID_THROW(claid::Exception, "Failed to parse PeriodicValue.\n"
                    << "Unknown unit \"" << unit << "\"\n");
                }
            }

        public:

            Reflect(PeriodicValue,
                reflectFunction(getPeriod);
                reflectFunction(getPeriodInMilliSeconds);
                reflectFunction(getFrequency);
                splitReflectInType(reflector, *this);
            )

            template<typename Reflector>
            void reflectRead(Reflector& r)
            {
                std::string rate;
                r.member("rate", rate, "");
                
                this->parseFromString(rate);
            }

            template<typename Reflector>
            void reflectWrite(Reflector& r)
            {
                std::string rate;
                if(this->wasSpecifiedAsFrequency)
                {
                    rate = std::to_string(1.0/this->periodInSeconds) + std::string("Hz");
                }
                r.member("rate", rate, "");
            }          

            double getPeriod()
            {
                return this->periodInSeconds;
            }

            double getFrequency()
            {
                return 1.0/this->periodInSeconds;
            }

            int getPeriodInMilliSeconds()
            {
                return this->periodInSeconds * 1000;
            }
    };
}