/*
	ASCIIEncoding.h - Definition of the ASCIIEncoding class.
*/


#ifndef Core_ASCIIEncoding_INCLUDED
#define Core_ASCIIEncoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {


class Foundation_API ASCIIEncoding: public TextEncoding
	/// 7-bit ASCII text encoding.
{
public:
	ASCIIEncoding();
	~ASCIIEncoding();
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


#endif // Core_ASCIIEncoding_INCLUDED
