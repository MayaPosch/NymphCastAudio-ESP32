/*
	Windows1251Encoding.h - Definition of the Windows1251Encoding class.
*/


#ifndef Core_Windows1251Encoding_INCLUDED
#define Core_Windows1251Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {


class Foundation_API Windows1251Encoding: public TextEncoding
	/// Windows Codepage 1251 text encoding.
	/// Based on: http://msdn.microsoft.com/en-us/goglobal/cc305144
{
public:
	Windows1251Encoding();
	~Windows1251Encoding();
	const char* canonicalName() const;
	bool isA(const std::string& encodingName) const;
	const CharacterMap& characterMap() const;
	int convert(const unsigned char* bytes) const;
	int convert(int ch, unsigned char* bytes, int length) const;
	int queryConvert(const unsigned char* bytes, int length) const;
	int sequenceLength(const unsigned char* bytes, int length) const;
	
private:
	static const char* _names[];
	static const CharacterMap _charMap;
};


} // namespace Poco


#endif // Core_Windows1251Encoding_INCLUDED