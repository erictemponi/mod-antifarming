#ifndef ANTIFARMING_H
#define ANTIFARMING_H

#include "ScriptMgr.h"
#include "Player.h"
#include "BanManager.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "cs_antifarming.h"
#include "Configuration/Config.h"

class AntiFarming {
public:
    typedef std::map<uint64, uint8> antiFarmingData;
    antiFarmingData dataMap;
};
#define sAntiFarming ACE_Singleton<AntiFarming, ACE_Null_Mutex>::instance()

#endif