/*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "Language.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Configuration/Config.h"

class antifarming_commandscript : public CommandScript
{
public:
    antifarming_commandscript() : CommandScript("antifarming_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override
    {

        static std::vector<ChatCommand> HelpDeleteSubCommandTable =
        {
            { "all",		SEC_GAMEMASTER,		false,	&HandleHelpDeleteAllCommand,	"" },
            { "ID",		    SEC_GAMEMASTER,		false,	&HandleHelpDeleteIDCommand,		"" }
        };

        static std::vector<ChatCommand> HelpCommandSubTable =
        {
            { "log",		SEC_GAMEMASTER,		false,	&HandleHelpLogCommand,		"" },
            { "delete",		SEC_GAMEMASTER,		false,	NULL,		            	"",	HelpDeleteSubCommandTable },
            { "clear",		SEC_GAMEMASTER,		false,	&HandleHelpClearCommand,	"" }
        };

        static std::vector<ChatCommand> DeleteCommandSubTable =
        {
            { "all",		SEC_GAMEMASTER,		true,	&HandleDeleteAllCommand,	"" },
            { "ID",		    SEC_GAMEMASTER,		true,	&HandleDeleteIDCommand,		"" }
        };

        static std::vector<ChatCommand> AFSSubCommandTable =
        {
            { "log",		SEC_GAMEMASTER,		true,	&HandleLogCommand,		    "" },
            { "delete",		SEC_ADMINISTRATOR,	true,	NULL,		            	"", DeleteCommandSubTable },
            { "help",		SEC_GAMEMASTER,		false,	NULL,		            	"", HelpCommandSubTable },
            { "editai",		SEC_ADMINISTRATOR,	true,	&HandleEditAICommand,		"" }
        };


        static std::vector<ChatCommand> commandTable =
        {
            { "afs",	    SEC_GAMEMASTER,		true,	NULL,		            	"",	AFSSubCommandTable }
        };
        return commandTable;
    }

    static bool HandleLogCommand(ChatHandler* handler, const char* args)
    {
        WorldSession * Session = handler->GetSession();
        Player* player = Session->GetPlayer();
        uint32 RLimit = 10;
        if ((char*)args)
            RLimit = atoi((char*)args);

        if (RLimit == 0 || RLimit > 100)
            RLimit = 10;

        QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `anti-farm_log` LIMIT %u", RLimit);
        if (!result)
        {
            Session->SendNotification("There are no records");
            return false;
        }
        Field * fields = NULL;
        do {
            fields = result->Fetch();
            uint32 ID = fields[0].GetUInt32();
            std::string Character = fields[1].GetString();
            std::string Account = fields[2].GetString();
            uint32 Warning = fields[3].GetUInt32();
            char msg[250];
            snprintf(msg, 250, "ID : %u Character : %s Account : %s Warning Level : %u \n", ID, Character.c_str(), Account.c_str(), Warning);
            handler->PSendSysMessage(msg);
            handler->SetSentErrorMessage(true);
        } while (result->NextRow());
        return true;
    }

    static bool HandleDeleteAllCommand(ChatHandler* handler, const char* args)
    {
        WorldSession * Session = handler->GetSession();
        Player* player = Session->GetPlayer();
        CharacterDatabase.Execute("DELETE FROM `anti-farm_log`;");
        QueryResult result = CharacterDatabase.Query("SELECT `id` FROM `anti-farm_log`");
        if (result)
        {
            Session->SendNotification("Deletion failed");
            return false;
        }
        else
        {
            Session->SendAreaTriggerMessage("Deletion successful!");
            return true;
        }
        return true;
    }

    static bool HandleDeleteIDCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        WorldSession * Session = handler->GetSession();
        Player* player = Session->GetPlayer();
        uint32 ID = atoi((char*)args);
        QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `anti-farm_log` WHERE `id` = %u", ID);
        if (!result)
        {
            char msg[250];
            snprintf(msg, 250, "Log with Log ID : %u doesn't exist", ID);
            Session->SendNotification(msg);
            return false;
        }
        CharacterDatabase.PExecute("DELETE FROM `anti-farm_log` WHERE `id` = %u", ID);
        Session->SendAreaTriggerMessage("Deletion of Log ID : %u Successful!", ID);
        return true;
    }

    static bool HandleHelpLogCommand(ChatHandler* handler, const char* args)
    {
        handler->PSendSysMessage("This command allows you to check the Database Log for abusers of the Anti-Farm System \n");
        handler->PSendSysMessage("Entering a number after this command will be used to check the maximum lines shown \n");
        handler->PSendSysMessage("Example : \".afs log 15\" will show the first 15 records, even if there are more.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteAllCommand(ChatHandler* handler, const char* args)
    {
        handler->PSendSysMessage("This command will delete all existing records of abusers \n");
        handler->PSendSysMessage("This command does not have any arguments.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleHelpDeleteIDCommand(ChatHandler* handler, const char* args)
    {
        handler->PSendSysMessage("This command will let you delete a specific record from the Database \n");
        handler->PSendSysMessage("Entering a number after this command will act like the ID of the record \n");
        handler->PSendSysMessage("Example : \".afs delete ID 50 \" will delete the record with ID 50.");
        handler->SetSentErrorMessage(true);
        return true;
    }
    static bool HandleHelpClearCommand(ChatHandler* handler, const char* args)
    {
        handler->SendSysMessage("This command edits the starting point of the Auto Incrementation for the ID column according to your argument");
        handler->SendSysMessage("this is usually used when the number of ID gets too high after \n the  records lower than the Auto Increment are deleted");
        handler->SendSysMessage("Example : \".afs editAI 50\" will edit the starting point of the Auto Incrementation to 50.");
        handler->SetSentErrorMessage(true);
        return true;
    }

    static bool HandleEditAICommand(ChatHandler* handler, const char* args)
    {
        uint32 AIedit = atoi((char*)args);
        WorldSession * Session = handler->GetSession();
        Player* player = Session->GetPlayer();
        CharacterDatabase.PExecute("ALTER TABLE `anti-farm_log` AUTO_INCREMENT = %u", AIedit);
        Session->SendAreaTriggerMessage("Auto Increment starting point successfully set to %u", AIedit);
        return true;
    }
};

void AddSC_antifarming_commandscript()
{
    new antifarming_commandscript();
}