#include <WString.h>

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667

struct IRCCallbackMessage {
    String nick;
    String text;
};

typedef void (*irc_callback)(IRCCallbackMessage message);
typedef String (*irc_config_callback)();

void irc_setup(irc_callback callback, irc_config_callback config_callback);
void irc_loop();