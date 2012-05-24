#include "GameTypes.h"

const _CRC32_4 CRC32_4 = (_CRC32_4)0x00A32040;
const _CRC32_8 CRC32_8 = (_CRC32_8)0x00A320A0;


StringCache * StringCache::GetSingleton(void)
{
	typedef StringCache * (* _GetSingleton)(void);

	return ((_GetSingleton)0x00A385D0)();
}

StringCache::Lock * StringCache::GetLock(UInt32 crc16)
{
	return &locks[crc16 & 0x1F];
}

StringCache::Ref::Ref(const char * buf)
{
	CALL_MEMBER_FN(this, ctor)(buf);
}

const char * BSString::Get(void)
{
	return m_data ? m_data : "";
}

