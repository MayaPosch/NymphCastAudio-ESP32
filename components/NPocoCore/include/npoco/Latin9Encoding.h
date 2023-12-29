/*
	Latin9Encoding.h - Definition of the Latin9Encoding class.
*/


#ifndef Core_Latin9Encoding_INCLUDED
#define Core_Latin9Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {

/// ISO Latin-9 (8859-15) text encoding.
///
/// Latin-9 is basically Latin-1 with the EURO sign plus
/// some other minor changes.
class Foundation_API Latin9Encoding: public TextEncoding {
public:
	Latin9Encoding();
	~Latin9Encoding();
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


#endif // Core_Latin9Encoding_INCLUDED
