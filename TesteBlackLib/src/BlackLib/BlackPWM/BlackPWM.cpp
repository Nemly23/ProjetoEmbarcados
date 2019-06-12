 /*

 ####################################################################################
 #  BlackLib Library controls Beaglebone Black's inputs and outputs.                #
 #  Copyright (C) 2013-2015 by Yigit YUCE                                           #
 ####################################################################################
 #                                                                                  #
 #  This file is part of BlackLib library.                                          #
 #                                                                                  #
 #  BlackLib library is free software: you can redistribute it and/or modify        #
 #  it under the terms of the GNU Lesser General Public License as published by     #
 #  the Free Software Foundation, either version 3 of the License, or               #
 #  (at your option) any later version.                                             #
 #                                                                                  #
 #  BlackLib library is distributed in the hope that it will be useful,             #
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of                  #
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   #
 #  GNU Lesser General Public License for more details.                             #
 #                                                                                  #
 #  You should have received a copy of the GNU Lesser General Public License        #
 #  along with this program.  If not, see <http://www.gnu.org/licenses/>.           #
 #                                                                                  #
 #  For any comment or suggestion please contact the creator of BlackLib Library    #
 #  at ygtyce@gmail.com                                                             #
 #                                                                                  #
 ####################################################################################

 */
#include "BlackPWM.h"
#include "stdint.h"

namespace BlackLib
{

    // ######################################### BLACKCOREPWM DEFINITION STARTS ########################################## //
    BlackCorePWM::BlackCorePWM(pwmName pwm)
    {
        this->pwmPinName    = pwm;
        this->pwmCoreErrors = new errorCorePWM( this->getErrorsFromCore() );
        this->expPath       = this->getexportpath(this->pwmPinName) + "/export";
        this->unExpPath       = this->getexportpath(this->pwmPinName) + "/unexport";
        this->modePath       = this->getmodepath(this->pwmPinName);

        this->exportPWM();

        this->loadDeviceTree();

        this->pwmTestPath   = this->findPwmTestName( this->pwmPinName );
    }


    BlackCorePWM::~BlackCorePWM()
    {
    	this->unexportPWM();
        delete this->pwmCoreErrors;
    }

    std::string        BlackCorePWM::getexportpath(pwmName pwm){
    	std::string searchResult = SEARCH_DIR_NOT_FOUND;
		switch (pwm)
		{
			case P8_13:
			{
				searchResult = "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip7";
				break;
			}

			case P8_19:
			{
				searchResult = "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip7";
				break;
			}

			case P9_14:
			{
				searchResult = "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip4";
				break;
			}

			case P9_16:
			{
				searchResult = "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip4";
				break;
			}

			case P9_21:
			{
				searchResult = "/sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/pwm/pwmchip1";
				break;
			}

			case P9_22:
			{
				searchResult = "/sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/pwm/pwmchip1";
				break;
			}
			case PWMDISABLE:
			{
				break;
			}
		};
		return searchResult;
    }

    std::string        BlackCorePWM::getmodepath(pwmName pwm){
		std::string searchResult = SEARCH_DIR_NOT_FOUND;
		switch (pwm)
		{
			case P8_13:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_13_pinmux/state";
				break;
			}

			case P8_19:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_19_pinmux/state";
				break;
			}

			case P9_14:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_14_pinmux/state";
				break;
			}

			case P9_16:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_16_pinmux/state";
				break;
			}

			case P9_21:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_21_pinmux/state";
				break;
			}

			case P9_22:
			{
				searchResult = "/sys/devices/platform/ocp/ocp:P8_22_pinmux/state";
				break;
			}
			case PWMDISABLE:
			{
				break;
			}
		};
		return searchResult;
	}

    bool        BlackCorePWM::exportPWM(){
    	 std::ofstream expFile;

    	expFile.open(this->expPath.c_str(),std::ios::out);
    	if(expFile.fail())
    	{
    		expFile.close();
			return false;
		}
		else
		{
			if (this->pwmPinName%2 == 1){
				expFile << 0;
			}
			else {
				expFile << 1;
			}
			expFile.close();
		}
    	expFile.open(this->modePath.c_str(),std::ios::out);
    	if(expFile.fail())
		{
			expFile.close();
			return false;
		}
		else
		{

			expFile << "pwm";
			expFile.close();
			return true;
		}

    }

    bool        BlackCorePWM::unexportPWM(){
		 std::ofstream expFile;

		 expFile.open(this->unExpPath.c_str(),std::ios::out);
		 if(expFile.fail())
		 {
		expFile.close();
			return false;
		}
		else
		{
			if (this->pwmPinName%2 == 1){
				expFile << 0;
			}
			else {
				expFile << 1;
			}
			expFile.close();
		}
		 expFile.open(this->modePath.c_str(),std::ios::out);
		if(expFile.fail())
		{
			expFile.close();
			return false;
		}
		else
		{

			expFile << "default";
			expFile.close();
			return true;
		}
	}

    bool        BlackCorePWM::loadDeviceTree()
    {
        std::string file    = this->getSlotsFilePath();
        std::ofstream slotsFile;

        slotsFile.open(file.c_str(), std::ios::out);
        if(slotsFile.fail())
        {
            slotsFile.close();
            this->pwmCoreErrors->dtSsError  = true;
            this->pwmCoreErrors->dtError    = true;
            return false;
        }
        else
        {
            slotsFile << "am33xx_pwm";
            slotsFile.close();
            this->pwmCoreErrors->dtSsError  = false;
        }


        slotsFile.open(file.c_str(), std::ios::out);
        if(slotsFile.fail())
        {
            slotsFile.close();
            this->pwmCoreErrors->dtError    = true;
            return false;
        }
        else
        {
            slotsFile << ("bone_pwm_" + pwmNameMap[this->pwmPinName]);
            slotsFile.close();
            this->pwmCoreErrors->dtError    = false;
            return true;
        }
    }

    std::string BlackCorePWM::findPwmTestName(pwmName pwm)
    {
        std::string searchResult = SEARCH_DIR_NOT_FOUND;
        switch (pwm)
        {
            case P8_13:
            {
                searchResult = "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip7/pwm-7:1";
                break;
            }

            case P8_19:
            {
                searchResult = "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip7/pwm-7:0";
                break;
            }

            case P9_16:
            {
                searchResult = "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip4/pwm-4:1";
                break;
            }

            case P9_14:
            {
                searchResult = "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip4/pwm-4:0";
                break;
            }

            case P9_21:
            {
                searchResult = "/sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/pwm/pwmchip1/pwm-1:1";
                break;
            }

            case P9_22:
            {
                searchResult = "/sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/pwm/pwmchip1/pwm-1:0";
                break;
            }

            case PWMDISABLE:
            {
                break;
            }
        };


        if( searchResult == SEARCH_DIR_NOT_FOUND )
        {
            this->pwmCoreErrors->pwmTestError = true;
            return PWM_TEST_NAME_NOT_FOUND;
        }
        else
        {
            this->pwmCoreErrors->pwmTestError = false;
            return searchResult;
        }
    }



    std::string BlackCorePWM::getPeriodFilePath()
    {
        return (this->pwmTestPath + "/period");
    }

    std::string BlackCorePWM::getDutyFilePath()
    {
        return (this->pwmTestPath + "/duty_cycle");
    }

    std::string BlackCorePWM::getRunFilePath()
    {
        return (this->pwmTestPath + "/enable");
    }

    std::string BlackCorePWM::getPolarityFilePath()
    {
        return (this->pwmTestPath + "/polarity");
    }

    errorCorePWM *BlackCorePWM::getErrorsFromCorePWM()
    {
        return (this->pwmCoreErrors);
    }
    // ########################################## BLACKCOREPWM DEFINITION ENDS ########################################### //












    // ########################################### BLACKPWM DEFINITION STARTS ############################################ //
    BlackPWM::BlackPWM(pwmName pwm) : BlackCorePWM(pwm)
    {
        this->pwmErrors     = new errorPWM( this->getErrorsFromCorePWM() );

        this->periodPath    = this->getPeriodFilePath();
        this->dutyPath      = this->getDutyFilePath();
        this->runPath       = this->getRunFilePath();
        this->polarityPath  = this->getPolarityFilePath();
    }

    BlackPWM::~BlackPWM()
    {
        delete this->pwmErrors;
    }

    std::string BlackPWM::getValue()
    {
        double period   = static_cast<long double>( this->getNumericPeriodValue() );
        double duty     = static_cast<long double>( this->getNumericDutyValue() );

        return tostr(static_cast<float>( (1.0 - (duty / period )) * 100 ));
    }

    std::string BlackPWM::getPeriodValue()
    {
        std::ifstream periodValueFile;

        periodValueFile.open(this->periodPath.c_str(),std::ios::in);
        if(periodValueFile.fail())
        {
            periodValueFile.close();
            this->pwmErrors->periodFileError = true;
            return FILE_COULD_NOT_OPEN_STRING;
        }
        else
        {
            std::string readValue;
            periodValueFile >> readValue;

            periodValueFile.close();
            this->pwmErrors->periodFileError = false;
            return readValue;
        }
    }

    std::string BlackPWM::getDutyValue()
    {
        std::ifstream dutyValueFile;

        dutyValueFile.open(this->dutyPath.c_str(),std::ios::in);
        if(dutyValueFile.fail())
        {
            dutyValueFile.close();
            this->pwmErrors->dutyFileError = true;
            return FILE_COULD_NOT_OPEN_STRING;
        }
        else
        {
            std::string readValue;
            dutyValueFile >> readValue;

            dutyValueFile.close();
            this->pwmErrors->dutyFileError = false;
            return readValue;
        }
    }

    std::string BlackPWM::getRunValue()
    {
        std::ifstream runValueFile;

        runValueFile.open(this->runPath.c_str(),std::ios::in);
        if(runValueFile.fail())
        {
            runValueFile.close();
            this->pwmErrors->runFileError = true;
            return FILE_COULD_NOT_OPEN_STRING;
        }
        else
        {
            std::string readValue;
            runValueFile >> readValue;

            runValueFile.close();
            this->pwmErrors->runFileError = false;
            return readValue;
        }
    }

    std::string BlackPWM::getPolarityValue()
    {
        std::ifstream polarityValueFile;

        polarityValueFile.open(this->polarityPath.c_str(),std::ios::in);
        if(polarityValueFile.fail())
        {
            polarityValueFile.close();
            this->pwmErrors->polarityFileError = true;
            return FILE_COULD_NOT_OPEN_STRING;
        }
        else
        {
            std::string readValue;
            polarityValueFile >> readValue;

            polarityValueFile.close();
            this->pwmErrors->polarityFileError = false;
            return readValue;
        }
    }

    float       BlackPWM::getNumericValue()
    {
        double period   = static_cast<long double>( this->getNumericPeriodValue() );
        double duty     = static_cast<long double>( this->getNumericDutyValue() );

        return static_cast<float>( (1.0 - (duty / period )) * 100 );
    }

    inline int64_t    BlackPWM::getNumericPeriodValue()
    {
        int64_t readValue = FILE_COULD_NOT_OPEN_INT;

        std::ifstream periodValueFile;

        periodValueFile.open(this->periodPath.c_str(),std::ios::in);
        if(periodValueFile.fail())
        {
            periodValueFile.close();
            this->pwmErrors->periodFileError = true;
        }
        else
        {
            periodValueFile >> readValue;

            periodValueFile.close();
            this->pwmErrors->periodFileError = false;
        }
        return readValue;
    }

    inline int64_t    BlackPWM::getNumericDutyValue()
    {
        int64_t readValue = FILE_COULD_NOT_OPEN_INT;
        std::ifstream dutyValueFile;

        dutyValueFile.open(this->dutyPath.c_str(),std::ios::in);
        if(dutyValueFile.fail())
        {
            dutyValueFile.close();
            this->pwmErrors->dutyFileError = true;
        }
        else
        {
            dutyValueFile >> readValue;

            dutyValueFile.close();
            this->pwmErrors->dutyFileError = false;
        }
        return readValue;
    }


    bool        BlackPWM::setDutyPercent(float percentage)
    {
        if( percentage > 100.0 or percentage < 0.0 )
        {
            this->pwmErrors->outOfRange      = true;
            this->pwmErrors->dutyFileError   = true;
            this->pwmErrors->periodFileError = true;
            return false;
        }

        this->pwmErrors->outOfRange = false;

        std::ofstream dutyFile;
        dutyFile.open(this->dutyPath.c_str(),std::ios::out);
        if(dutyFile.fail())
        {
            dutyFile.close();
            this->pwmErrors->dutyFileError = true;
            return false;
        }
        else
        {
            dutyFile << static_cast<int64_t>(std::floor((this->getNumericPeriodValue()) * (1.0 - (percentage/100))));
            dutyFile.close();
            this->pwmErrors->dutyFileError = false;
            return true;
        }

    }

    bool        BlackPWM::setPeriodTime(uint64_t period, timeType tType)
    {
        uint64_t writeThis = static_cast<uint64_t>(period * static_cast<double>(pow( 10, static_cast<int>(tType)+9) ));

        if( writeThis > 1000000000)
        {
            this->pwmErrors->outOfRange = true;
            return false;
        }
        else
        {
            this->pwmErrors->outOfRange = false;
            std::ofstream periodFile;
            periodFile.open(this->periodPath.c_str(),std::ios::out);
            if(periodFile.fail())
            {
                periodFile.close();
                this->pwmErrors->periodFileError = true;
                return false;
            }
            else
            {
                periodFile << writeThis;
                periodFile.close();
                this->pwmErrors->periodFileError = false;
                return true;
            }
        }

    }

    bool        BlackPWM::setSpaceRatioTime(uint64_t space, timeType tType)
    {
        uint64_t writeThis = static_cast<int64_t>(space * static_cast<double>(pow( 10, static_cast<int>(tType)+9) ));

        if( writeThis > 1000000000)
        {
            this->pwmErrors->outOfRange = true;
            return false;
        }
        else
        {
            std::ofstream dutyFile;
            dutyFile.open(this->dutyPath.c_str(),std::ios::out);
            if(dutyFile.fail())
            {
                dutyFile.close();
                this->pwmErrors->dutyFileError = true;
                return false;
            }
            else
            {
                dutyFile << writeThis;
                dutyFile.close();
                this->pwmErrors->dutyFileError = false;
                return true;
            }
        }
    }

    bool        BlackPWM::setLoadRatioTime(uint64_t load, timeType tType)
    {
        uint64_t writeThis = (this->getNumericPeriodValue() - static_cast<int64_t>(load * static_cast<double>(pow( 10, static_cast<int>(tType)+9) )));

        if( writeThis > 1000000000)
        {
            this->pwmErrors->outOfRange = true;
            return false;
        }
        else
        {
            std::ofstream dutyFile;
            dutyFile.open(this->dutyPath.c_str(),std::ios::out);
            if(dutyFile.fail())
            {
                dutyFile.close();
                this->pwmErrors->dutyFileError = true;
                return false;
            }
            else
            {
                dutyFile << writeThis;
                dutyFile.close();
                this->pwmErrors->dutyFileError = false;
                return true;
            }
        }
    }

    bool        BlackPWM::setPolarity(polarityType polarity)
    {
        std::ofstream polarityFile;
        polarityFile.open(this->polarityPath.c_str(),std::ios::out);
        if(polarityFile.fail())
        {
            polarityFile.close();
            this->pwmErrors->polarityFileError = true;
            return false;
        }
        else
        {
            polarityFile << static_cast<int>(polarity);
            polarityFile.close();
            this->pwmErrors->polarityFileError = false;
            return true;
        }
    }

    bool        BlackPWM::setRunState(runValue state)
    {
        std::ofstream runFile;
        runFile.open(this->runPath.c_str(),std::ios::out);
        if(runFile.fail())
        {
            runFile.close();
            this->pwmErrors->runFileError = true;
            return false;
        }
        else
        {
            runFile << static_cast<int>(state);
            runFile.close();
            this->pwmErrors->runFileError = false;
            return true;
        }
    }



    bool        BlackPWM::isRunning()
    {
        return (this->getRunValue() == "1");
    }

    bool        BlackPWM::isPolarityStraight()
    {
        return !(this->getPolarityValue() == "1");
    }

    bool        BlackPWM::isPolarityReverse()
    {
        return (this->getPolarityValue() == "1");
    }



    void        BlackPWM::toggleRunState()
    {
        if( this->getRunValue() == "1" )
        {
            this->setRunState(stop);
        }
        else
        {
            this->setRunState(run);
        }
    }

    void        BlackPWM::tooglePolarity()
    {
        if( this->getPolarityValue() == "0" )
        {
            this->setPolarity(reverse);
        }
        else
        {
            this->setPolarity(straight);
        }
    }

    bool        BlackPWM::fail()
    {
        return (this->pwmErrors->outOfRange or
                this->pwmErrors->runFileError or
                this->pwmErrors->dutyFileError or
                this->pwmErrors->periodFileError or
                this->pwmErrors->polarityFileError or
                this->pwmErrors->pwmCoreErrors->dtError or
                this->pwmErrors->pwmCoreErrors->dtSsError or
                this->pwmErrors->pwmCoreErrors->pwmTestError or
                this->pwmErrors->pwmCoreErrors->coreErrors->ocpError or
                this->pwmErrors->pwmCoreErrors->coreErrors->capeMgrError
                );
    }

    bool        BlackPWM::fail(BlackPWM::flags f)
    {
        if(f==outOfRangeErr)    { return this->pwmErrors->outOfRange;                               }
        if(f==runFileErr)       { return this->pwmErrors->runFileError;                             }
        if(f==dutyFileErr)      { return this->pwmErrors->dutyFileError;                            }
        if(f==periodFileErr)    { return this->pwmErrors->periodFileError;                          }
        if(f==polarityFileErr)  { return this->pwmErrors->polarityFileError;                        }
        if(f==dtErr)            { return this->pwmErrors->pwmCoreErrors->dtError;                   }
        if(f==dtSubSystemErr)   { return this->pwmErrors->pwmCoreErrors->dtSsError;                 }
        if(f==pwmTestErr)       { return this->pwmErrors->pwmCoreErrors->pwmTestError;              }
        if(f==ocpErr)           { return this->pwmErrors->pwmCoreErrors->coreErrors->ocpError;      }
        if(f==cpmgrErr)         { return this->pwmErrors->pwmCoreErrors->coreErrors->capeMgrError;  }

        return true;
    }

    // ########################################### BLACKPWM DEFINITION STARTS ############################################ //


} /* namespace BlackLib */
