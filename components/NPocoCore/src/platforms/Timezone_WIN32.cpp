/*
	Timezone_WIN32.cpp - 
*/


#include "../Timezone.h"
#include "../UnicodeConverter.h"
//#include "Poco/Exception.h"
#include "UnWindows.h"
#include <ctime>


namespace Poco {


int Timezone::utcOffset()
{
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	return -tzInfo.Bias*60;
}

	
int Timezone::dst()
{
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	return dstFlag == TIME_ZONE_ID_DAYLIGHT ? -tzInfo.DaylightBias*60 : 0;
}


bool Timezone::isDst(const Timestamp& timestamp)
{
	std::time_t time = timestamp.epochTime();
	struct std::tm* tms = std::localtime(&time);
	if (!tms) { //throw Poco::SystemException("cannot get local time DST flag");
		return false;
	}
	
	return tms->tm_isdst > 0;
}

	
std::string Timezone::name()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = dstFlag == TIME_ZONE_ID_DAYLIGHT ? tzInfo.DaylightName : tzInfo.StandardName;
	UnicodeConverter::toUTF8(ptr, result);
	return result;
}

	
std::string Timezone::standardName()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = tzInfo.StandardName;
	UnicodeConverter::toUTF8(ptr, result);
	return result;
}

	
std::string Timezone::dstName()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = tzInfo.DaylightName;
	UnicodeConverter::toUTF8(ptr, result);
	return result;
}


} // namespace Poco
