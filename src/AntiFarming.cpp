/*
- Made by mthsena -
- Edited by Jameyboor -
- Fixed by Dawnbringers -
- Adapted by Erictemponi -
*/

#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "cs_antifarming.h"
#include "Configuration/Config.h"

class AntiFarmingPlayerScript : public PlayerScript {
public: AntiFarmingPlayerScript() : PlayerScript("AntiFarmingPlayerScript") {}

        void OnLogin(Player* player) override {
            if (sConfigMgr->GetBoolDefault("AntiFarming.Enable", true)) {
                if (sConfigMgr->GetBoolDefault("AntiFarming.LoginMessage", true))
                    ChatHandler(player->GetSession()).PSendSysMessage("This server is running an Antifarming Module.");
            }
        }

        void OnPVPKill(Player* killer, Player* killed) {
            if (sConfigMgr->GetBoolDefault("AntiFarming.Enable", true)) {
                if (killer->GetGUID() == killed->GetGUID())
                    return;
				
                if (killer->GetSession()->GetRemoteAddress() == killed->GetSession()->GetRemoteAddress() || killed->GetMaxHealth() < 10000) { // about 1/4 of the maximum hp a character can have on your server to make this fully functional.
                    uint32 Warning = 1;
                    std::string Acc;
                    std::string Char = killer->GetName();
                    std::string str = "";
                    str = "|cFFFFFC00[Anti-Farm System]|cFF00FFFF[|cFF60FF00" + Char + "|cFF00FFFF] Possible Farmer!";
                    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
                    data << str;
                    sWorld->SendGlobalGMMessage(&data);
                    
                    QueryResult resultDB = CharacterDatabase.PQuery("SELECT `warnings` FROM `anti-farm_log` WHERE `character` = \"%s\";", Char.c_str());
                    if (resultDB) {
                        Field *fields = resultDB->Fetch();
                        Warning = fields[0].GetUInt32();
                    }
                    resultDB = LoginDatabase.PQuery("SELECT `username` FROM `account` WHERE `id` = %u;", killer->GetSession()->GetAccountId());
                    if (resultDB) {
                        Field* fields = resultDB->Fetch();
                        Acc = fields[0].GetString();
                    }
                    resultDB = CharacterDatabase.PQuery("SELECT `id` FROM `anti-farm_log` WHERE `character` = \"%s\";", Char.c_str());
                    if (resultDB)
                        CharacterDatabase.PExecute("UPDATE `anti-farm_log` SET `warnings` = warnings + 1 WHERE `character` = \"%s\";", Char.c_str());
                    else
                        CharacterDatabase.PExecute("INSERT INTO `anti-farm_log` (`character`, `account`, `warnings`) VALUES (\"%s\", \"%s\", %u);", Char.c_str(), Acc.c_str(), Warning);

                    if (sConfigMgr->GetBoolDefault("AntiFarming.BanCharacter", true)) {
                        std::string banTime = sConfigMgr->GetStringDefault("AntiFarming.BanTime", "1") + "d";
                        resultDB = CharacterDatabase.PQuery("SELECT `warnings` FROM `anti-farm_log` WHERE `character` = \"%s\" AND `warnings` >= %u;", Char.c_str(), sConfigMgr->GetIntDefault("AntiFarming.Warnings", 5));
                        if (resultDB)
                            sWorld->BanCharacter(Char, banTime, "Ignored warnings of the Anti-Farm System", "Anti-Farm System");
                    }
                }
            }
        }
};

class AntiFarmingWorldScript : public WorldScript {
public:
    AntiFarmingWorldScript() : WorldScript("AntiFarmingWorldScript") { }

    void OnBeforeConfigLoad(bool reload) override {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/AntiFarming.conf";
            #ifdef WIN32
                cfg_file = "AntiFarming.conf";
            #endif // WIN32
            std::string cfg_def_file = cfg_file + ".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
    void OnAfterConfigLoad(bool reload) override {
        sLog->outString("Antifarming Module Loaded.");
    }
};

void AddAntiFarmingScripts()
{
    new AntiFarmingPlayerScript();
    new AntiFarmingWorldScript();
    AddSC_antifarming_commandscript();
}