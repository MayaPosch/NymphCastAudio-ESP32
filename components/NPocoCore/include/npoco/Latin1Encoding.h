/*
	Latin1Encoding.h - Definition of the Latin1Encoding class.
*/


#ifndef Core_Latin1Encoding_INCLUDED
#define Core_Latin1Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {

/// ISO Latin-1 (8859-1) text encoding.
class Foundation_API Latin1Encoding: public TextEncoding {
public:
	Latin1Encoding();
	~Latin1Encoding();
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


#endif // Core_Latin1Encoding_INCLUDED
