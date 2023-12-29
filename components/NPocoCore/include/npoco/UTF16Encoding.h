/*
	UTF16Encoding.h - Definition of the UTF16Encoding class.
*/


#ifndef Core_UTF16Encoding_INCLUDED
#define Core_UTF16Encoding_INCLUDED


#include "Core.h"
#include "TextEncoding.h"


namespace Poco {

/// UTF-16 text encoding, as defined in RFC 2781.
///
/// When converting from UTF-16 to Unicode, surrogates are
/// reported as they are - in other words, surrogate pairs
/// are not combined into one Unicode character. 
/// When converting from Unicode to UTF-16, however, characters
/// outside the 16-bit range are converted into a low and
/// high surrogate.
class Foundation_API UTF16Encoding: public TextEncoding {
public:
	enum ByteOrderType {
		BIG_ENDIAN_BYTE_ORDER,
		LITTLE_ENDIAN_BYTE_ORDER,
		NATIVE_BYTE_ORDER
	};
	
	UTF16Encoding(ByteOrderType byteOrder = NATIVE_BYTE_ORDER);
		/// Creates and initializes the encoding for the given byte order.
		
	UTF16Encoding(int byteOrderMark);
		/// Creates and initializes the encoding for the byte-order
		/// indicated by the given byte-order mark, which is the Unicode
		/// character 0xFEFF.
		
	~UTF16Encoding();
	
	ByteOrderType getByteOrder() const;
		/// Returns the byte-order currently in use.
		
	void setByteOrder(ByteOrderType byteOrder);
		/// Sets the byte order.
		
	void setByteOrder(int byteOrderMark);
		/// Sets the byte order according to the given
		/// byte order mark, which is the Unicode
		/// character 0xFEFF.
	
	const char* canonicalName() const;
	bool isA(const std::string& encodingName) const;
	const CharacterMap& characterMap() const;
	int convert(const unsigned char* bytes) const;
	int convert(int ch, unsigned char* bytes, int length) const;
	int queryConvert(const unsigned char* bytes, int length) const;
	int sequenceLength(const unsigned char* bytes, int length) const;
	
private:
	bool _flipBytes;
	static const char* _names[];
	static const CharacterMap _charMap;
};


} // namespace Poco


#endif // Core_UTF16Encoding_INCLUDED
