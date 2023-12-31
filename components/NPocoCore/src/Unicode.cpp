/*
	Unicode.cpp
*/


#include "Unicode.h"


extern "C"
{
#include "pcre_config.h"
#include "pcre_internal.h"
}


namespace Poco {


void Unicode::properties(int ch, CharacterProperties& props)
{
	if (ch > UCP_MAX_CODEPOINT) ch = 0;
	const ucd_record* ucd = GET_UCD(ch);
	props.category = static_cast<CharacterCategory>(_pcre_ucp_gentype[ucd->chartype]);
	props.type     = static_cast<CharacterType>(ucd->chartype);
	props.script   = static_cast<Script>(ucd->script);
}


int Unicode::toLower(int ch)
{
	if (isUpper(ch))
		return static_cast<int>(UCD_OTHERCASE(static_cast<unsigned>(ch)));
	else
		return ch;
}


int Unicode::toUpper(int ch)
{
	if (isLower(ch))
		return static_cast<int>(UCD_OTHERCASE(static_cast<unsigned>(ch)));
	else
		return ch;
}


} // namespace Poco
