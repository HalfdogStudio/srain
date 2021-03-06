/**
 * @file sirc_cmd.c
 * @brief Send IRC command to server
 * @author Shengyu Zhang <silverrainz@outlook.com>
 * @version 1.0
 * @date 2016-03-01
 *
 * Copied from <https://github.com/Themaister/simple-irc-bot>
 *
 */

#define __LOG_ON

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include "sirc/sirc.h"
#include "io_stream.h"

#include "srain.h"
#include "log.h"

int sirc_cmd_ping(SircSession *sirc, const char *data){
    g_return_val_if_fail(data, SRN_ERR);

    return sirc_cmd_raw(sirc, "PING :%s\r\n", data);
}

// sirc_cmd_pong: For answering pong requests...
int sirc_cmd_pong(SircSession *sirc, const char *data){
    g_return_val_if_fail(data, SRN_ERR);

    return sirc_cmd_raw(sirc, "PONG :%s\r\n", data);
}

int sirc_cmd_user(SircSession *sirc, const char *username, const char *hostname,
        const char *servername, const char *realname){
    g_return_val_if_fail(username, SRN_ERR);
    g_return_val_if_fail(hostname, SRN_ERR);
    g_return_val_if_fail(servername, SRN_ERR);
    g_return_val_if_fail(realname, SRN_ERR);

    return sirc_cmd_raw(sirc, "USER %s %s %s :%s\r\n",
            username, hostname, servername, realname);
}

// sirc_cmd_join: For joining a chan
int sirc_cmd_join(SircSession *sirc, const char *chan, const char *passwd){
    g_return_val_if_fail(chan, SRN_ERR);

    if (passwd) {
        return sirc_cmd_raw(sirc, "JOIN %s :%s\r\n", chan, passwd);
    } else {
        return sirc_cmd_raw(sirc, "JOIN %s\r\n", chan);
    }
}

// sirc_cmd_part: For leaving a chan
int sirc_cmd_part(SircSession *sirc, const char *chan, const char *reason){
    g_return_val_if_fail(chan, SRN_ERR);

    if (reason) {
        return sirc_cmd_raw(sirc, "PART %s :%s\r\n", chan, reason);
    } else {
        return sirc_cmd_raw(sirc, "PART %s\r\n", chan);
    }
}

// sirc_cmd_nick: For changing your nick
int sirc_cmd_nick(SircSession *sirc, const char *nick){
    g_return_val_if_fail(nick, SRN_ERR);

    return sirc_cmd_raw(sirc, "NICK %s\r\n", nick);
}

// sirc_cmd_quit: For quitting IRC
int sirc_cmd_quit(SircSession *sirc, const char *reason){
    g_return_val_if_fail(reason, SRN_ERR);

    return sirc_cmd_raw(sirc, "QUIT :%s\r\n", reason);
}


// sirc_cmd_topic: For setting or removing a topic
int sirc_cmd_topic(SircSession *sirc, const char *chan, const char *topic){
    g_return_val_if_fail(chan, SRN_ERR);
    g_return_val_if_fail(topic, SRN_ERR);

    return sirc_cmd_raw(sirc, "TOPIC %s :%s\r\n", chan, topic);
}

// sirc_cmd_action: For executing an action (.e.g /me is hungry)
int sirc_cmd_action(SircSession *sirc, const char *chan, const char *msg){
    g_return_val_if_fail(chan, SRN_ERR);
    g_return_val_if_fail(msg, SRN_ERR);

    return sirc_cmd_raw(sirc, "PRIVMSG %s :\001ACTION %s\001\r\n", chan, msg);
}

// sirc_cmd_msg: For sending a chan message or a query
int sirc_cmd_msg(SircSession *sirc, const char *chan, const char *msg){
    g_return_val_if_fail(chan, SRN_ERR);
    g_return_val_if_fail(msg, SRN_ERR);

    return sirc_cmd_raw(sirc, "PRIVMSG %s :%s\r\n", chan, msg);
}

int sirc_cmd_names(SircSession *sirc, const char *chan){
    g_return_val_if_fail(chan, SRN_ERR);

    return sirc_cmd_raw(sirc, "NAMES %s\r\n", chan);
}

int sirc_cmd_whois(SircSession *sirc, const char *who){
    g_return_val_if_fail(who, SRN_ERR);

    return sirc_cmd_raw(sirc, "WHOIS %s\r\n", who);
}

int sirc_cmd_invite(SircSession *sirc, const char *nick, const char *chan){
    g_return_val_if_fail(nick, SRN_ERR);
    g_return_val_if_fail(chan, SRN_ERR);

    return sirc_cmd_raw(sirc, "INVITE %s %s\r\n", nick, chan);
}

int sirc_cmd_kick(SircSession *sirc, const char *nick, const char *chan,
        const char *reason){
    g_return_val_if_fail(nick, SRN_ERR);
    g_return_val_if_fail(chan, SRN_ERR);

    if (reason){
        return sirc_cmd_raw(sirc, "KICK %s %s :%s\r\n", chan, nick, reason);
    } else {
        return sirc_cmd_raw(sirc, "KICK %s %s\r\n", chan, nick);
    }
}

int sirc_cmd_mode(SircSession *sirc, const char *target, const char *mode){
    g_return_val_if_fail(target, SRN_ERR);
    g_return_val_if_fail(mode, SRN_ERR);

    return sirc_cmd_raw(sirc, "MODE %s %s\r\n", target, mode);
}

int sirc_cmd_pass(SircSession *sirc, const char *pass){
    g_return_val_if_fail(pass, SRN_ERR);

    return sirc_cmd_raw(sirc, "PASS %s\r\n", pass);
}

int sirc_cmd_raw(SircSession *sirc, const char *fmt, ...){
    char buf[SIRC_BUF_LEN];
    int len = 0;
    va_list args;
    GIOStream *stream;

    g_return_val_if_fail(sirc, SRN_ERR);
    g_return_val_if_fail(fmt, SRN_ERR);
    stream = sirc_get_stream(sirc);
    g_return_val_if_fail(G_IS_IO_STREAM(stream), SRN_ERR);

    if (strlen(fmt) != 0){
        va_start(args, fmt);
        len = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
    }

    if (len > 512){
        WARN_FR("Raw command too long");
        len = 512;
    }

    // TODO send it totally

    return (io_stream_write(stream, buf, len) < 0) ?
        SRN_ERR : SRN_OK;
}
