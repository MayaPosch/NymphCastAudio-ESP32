/*
	Windows1252Encoding.h - Definition of the Windows1252Encoding class.
*/


#ifndef Core_Windows1252Encoding_INCLUDED
#define Core_Windows1252Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {


class Foundation_API Windows1252Encoding: public TextEncoding
	/// Windows Codepage 1252 text encoding.
{
public:
	Windows1252Encoding();
	~Windows1252Encoding();
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


#endif // Core_Windows1252Encoding_INCLUDED
