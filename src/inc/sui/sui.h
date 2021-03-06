#ifndef __SUI_H
#define __SUI_H

#include <time.h>
#include "srain.h"

typedef struct _SuiSession SuiSession;
typedef int SuiSessionFlag;
typedef struct _SuiMessage SuiMessage;
typedef enum _UserType UserType;

#define SUI_SESSION_SERVER      1 << 0
#define SUI_SESSION_CHANNEL     1 << 1
#define SUI_SESSION_DIALOG      1 << 2
#define SUI_SESSION_NEWWINDOW   1 << 3  // Not used yet

// TODO Rename type
typedef enum {
    SYS_MSG_NORMAL,
    SYS_MSG_ERROR,
    SYS_MSG_ACTION
} SysMsgType;


enum _UserType {
    USER_OWNER,     // ~ mode +q
    USER_ADMIN,     // & mode +a
    USER_FULL_OP,   // @ mode +o
    USER_HALF_OP,   // % mode +h
    USER_VOICED,    // + mode +v
    USER_CHIGUA,    // No prefix
    /* ... */
    USER_TYPE_MAX
};

#define SRAIN_MSG_MENTIONED 0x1

#define __IN_SUI_H
#include "sui_event.h"
#include "sui_prefs.h"
#undef __IN_SUI_H

void sui_main_loop(SuiAppEvents *events, SuiAppPrefs *prefs);
void sui_proc_pending_event();

/* SuiSession */
SuiSession *sui_new_session(SuiEvents *events, SuiPrefs *prefs, SuiSessionFlag flag);
void sui_free_session(SuiSession *sui);
int sui_start_session(SuiSession *sui, const char *name, const char *remark);
void sui_end_session(SuiSession *sui);

SuiSessionFlag sui_get_flag(SuiSession *sui);
SuiEvents *sui_get_events(SuiSession *sui);
void* sui_get_ctx(SuiSession *sui);
void sui_set_ctx(SuiSession *sui, void *ctx);
void sui_set_name(SuiSession *sui, const char *name);
void sui_set_remark(SuiSession *sui, const char *remark);

/* SuiMessage */

void sui_message_set_ctx(SuiMessage *smsg, void *ctx);
void *sui_message_get_ctx(SuiMessage *smsg);
// TODO: pass a image file path
void sui_message_set_time(SuiMessage *smsg, time_t time);
void sui_message_append_image(SuiMessage *smsg, const char *url);
void sui_message_mentioned(SuiMessage *smsg);
void sui_message_notify(SuiMessage *smsg);

SuiMessage *sui_add_sys_msg(SuiSession *sui, const char *msg, SysMsgType type);
SuiMessage *sui_add_sent_msg(SuiSession *sui, const char *msg);
SuiMessage *sui_add_recv_msg(SuiSession *sui, const char *nick, const char *id, const char *msg);
void sui_message_append_message(SuiSession *sui, SuiMessage *smsg, const char *msg);

/* Completion */
void sui_add_completion(SuiSession *sui, const char *word);
void sui_rm_completion(SuiSession *sui, const char *word);

/* User */
int sui_add_user(SuiSession *sui, const char *nick, UserType type);
int sui_rm_user(SuiSession *sui, const char *nick);
int sui_ren_user(SuiSession *sui, const char *old_nick, const char *new_nick, UserType type);

/* Misc */
void sui_set_topic(SuiSession *sui, const char *topic);
void sui_message_box(const char *title, const char *msg);

#endif /* __SUI_H */
