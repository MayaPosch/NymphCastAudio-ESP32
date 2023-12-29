/*
	Latin2Encoding.h - Definition of the Latin2Encoding class.
*/


#ifndef Core_Latin2Encoding_INCLUDED
#define Core_Latin2Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {

/// ISO Latin-2 (8859-2) text encoding.
///
/// Latin-2 is basically Latin-1 with the EURO sign plus
/// some other minor changes.
class Foundation_API Latin2Encoding: public TextEncoding {
public:
	Latin2Encoding();
	virtual ~Latin2Encoding();
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


#endif // Foundation_Latin2Encoding_INCLUDED
