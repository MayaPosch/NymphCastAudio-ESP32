/*
	Windows1250Encoding.h - Definition of the Windows1250Encoding class.
*/


#ifndef Core_Windows1250Encoding_INCLUDED
#define Core_Windows1250Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {


class Foundation_API Windows1250Encoding: public TextEncoding
	/// Windows Codepage 1250 text encoding.
	/// Based on: http://msdn.microsoft.com/en-us/goglobal/cc305143
{
public:
	Windows1250Encoding();
	~Windows1250Encoding();
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


#endif // Core_Windows1250Encoding_INCLUDED
