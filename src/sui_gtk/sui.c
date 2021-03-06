/* @file sui.c
 * @brief GTK UI module interfaces
 * @author Shengyu Zhang <silverrainz@outlook.com>
 * @version 1.0
 * @date 2016-06-29
 */

#define __LOG_ON
#define __DBG_ON

#include <gtk/gtk.h>
#include <string.h>

#include "sui/sui.h"

#include "sui_common.h"
#include "srain_app.h"
#include "srain_window.h"
#include "theme.h"
#include "snotify.h"

#include "srain.h"
#include "i18n.h"
#include "log.h"
#include "meta.h"

struct _SuiSession{
    SrainChat *chat;

    SuiSessionFlag flag;
    SuiEvents *events;
    SuiPrefs *prefs;

    void *ctx;
};

bool is_app_run = FALSE;
SuiAppEvents *app_events = NULL;
SuiAppPrefs *app_prefs = NULL;

void sui_main_loop(SuiAppEvents *events, SuiAppPrefs *prefs){
    g_return_if_fail(events);
    g_return_if_fail(prefs);

    app_events = events;
    app_prefs = prefs;

    snotify_init();

    if (theme_load(prefs->theme) == SRN_ERR){
        char *errmsg = g_strdup_printf(_("Failed to load theme '%s'"), prefs->theme);
        ERR_FR(errmsg);
        sui_message_box(_("Error"), errmsg);
        g_free(errmsg);
    }

    is_app_run = TRUE;
    g_application_run(G_APPLICATION(srain_app_new()), 0, NULL);
    is_app_run = FALSE;

    snotify_finalize();
}

void sui_proc_pending_event(){
    while (gtk_events_pending()) gtk_main_iteration();
}

SuiSession *sui_new_session(SuiEvents *events, SuiPrefs *prefs, SuiSessionFlag flag){
    SuiSession *sui;

    g_return_val_if_fail(events, NULL);
    g_return_val_if_fail(prefs, NULL);

    sui = g_malloc0(sizeof(SuiSession));

    // sui->chat = NULL; // via g_malloc0()
    sui->flag = flag;
    sui->events = events;
    sui->prefs = prefs;

    return sui;
}

void sui_free_session(SuiSession *sui){
    g_return_if_fail(sui);

    if (sui->chat){
        srain_window_rm_chat(srain_win, sui->chat);
    }
    g_free(sui);
}

int sui_start_session(SuiSession *sui, const char *name, const char *remark){
    ChatType type;
    SuiSessionFlag flag;

    g_return_val_if_fail(sui, SRN_ERR);
    g_return_val_if_fail(name, SRN_ERR);
    g_return_val_if_fail(remark, SRN_ERR);

    flag = sui->flag;
    if (flag & SUI_SESSION_SERVER) {
        type = CHAT_SERVER;
    }
    else if (flag & SUI_SESSION_CHANNEL) {
        type = CHAT_CHANNEL;
    }
    else if (flag & SUI_SESSION_DIALOG) {
        type = CHAT_PRIVATE;
    } else {
        ERR_FR("Chat type not found in SuiSessionFlag 0x%x", flag);
        return SRN_ERR;
    }

    sui->chat = srain_window_add_chat(srain_win, sui, name, remark, type);

    if (!sui->chat){
        return SRN_ERR;
    }

    srain_chat_show_topic(sui->chat, sui->prefs->show_topic);
    srain_chat_show_user_list(sui->chat, sui->prefs->show_user_list);

    return SRN_OK;
}

void sui_end_session(SuiSession *sui){
    g_return_if_fail(sui);

    srain_window_rm_chat(srain_win, sui->chat);
    sui->chat = NULL;
}

SuiSessionFlag sui_get_flag(SuiSession *sui){
    /* Don't return SRN_ERR(-1 0xffffffff) */
    g_return_val_if_fail(sui, 0);

    return sui->flag;
}

SuiEvents *sui_get_events(SuiSession *sui){
    g_return_val_if_fail(sui, NULL);

    return sui->events;
}

SuiPrefs *sui_get_prefs(SuiSession *sui){
    g_return_val_if_fail(sui, NULL);

    return sui->prefs;
}

void* sui_get_ctx(SuiSession *sui){
    g_return_val_if_fail(sui, NULL);

    return sui->ctx;
}

void sui_set_ctx(SuiSession *sui, void *ctx){
    g_return_if_fail(sui);

    sui->ctx = ctx;
}

void sui_set_name(SuiSession *sui, const char *name){
    g_return_if_fail(sui);
    g_return_if_fail(name);

    srain_chat_set_name(sui->chat, name);
}

void sui_set_remark(SuiSession *sui, const char *remark){
    g_return_if_fail(sui);
    g_return_if_fail(remark);

    srain_chat_set_remark(sui->chat, remark);
}

SuiMessage *sui_add_sys_msg(SuiSession *sui, const char *msg, SysMsgType type){
    SuiMessage *smsg;
    SrainChat *chat;
    SrainMsgList *list;

    g_return_val_if_fail(sui, NULL);
    g_return_val_if_fail(msg, NULL);

    chat = sui->chat;
    g_return_val_if_fail(SRAIN_IS_CHAT(chat), NULL);

    list = srain_chat_get_msg_list(chat);
    smsg = (SuiMessage *)srain_sys_msg_new(msg, type);
    sui_message_set_ctx(smsg, sui);
    srain_msg_list_add_message(list, smsg);

    if (type != SYS_MSG_NORMAL){
        srain_window_stack_sidebar_update(srain_win, chat, NULL, msg);
    }

    return smsg;
}

SuiMessage *sui_add_sent_msg(SuiSession *sui, const char *msg){
    SrainChat *chat;
    SrainMsgList *list;
    SuiMessage *smsg;

    g_return_val_if_fail(msg, NULL);

    g_return_val_if_fail(sui, NULL);
    chat = sui->chat;

    g_return_val_if_fail(SRAIN_IS_CHAT(chat), NULL);

    list = srain_chat_get_msg_list(chat);
    smsg = (SuiMessage *)srain_send_msg_new(msg);
    sui_message_set_ctx(smsg, sui);
    srain_msg_list_add_message(list, smsg);

    srain_window_stack_sidebar_update(srain_win, chat, _("You"), msg);

    return smsg;
}

SuiMessage *sui_add_recv_msg(SuiSession *sui, const char *nick, const char *id,
        const char *msg){
    SrainChat *chat;
    SrainMsgList *list;
    SrainEntryCompletion *comp;
    SuiMessage *smsg;

    g_return_val_if_fail(sui, NULL);
    g_return_val_if_fail(nick, NULL);
    g_return_val_if_fail(msg, NULL);
    if (!id) id = "";

    chat = sui->chat;
    g_return_val_if_fail(SRAIN_IS_CHAT(chat), NULL);

    list = srain_chat_get_msg_list(chat);
    smsg = (SuiMessage *)srain_recv_msg_new(nick, id, msg);
    sui_message_set_ctx(smsg, sui);
    srain_recv_msg_show_avatar(SRAIN_RECV_MSG(smsg), sui->prefs->show_avatar);
    srain_msg_list_add_message(list, smsg);

    srain_window_stack_sidebar_update(srain_win, chat, nick, msg);
    if (strlen(id) != 0){
        comp = srain_chat_get_entry_completion(chat);
        srain_entry_completion_add_keyword(comp, nick, KEYWORD_TMP);
    }

    return smsg;
}

int sui_add_user(SuiSession *sui, const char *nick, UserType type){
    int res;
    SrainChat *chat;
    SrainUserList *list;
    SrainEntryCompletion *comp;

    g_return_val_if_fail(nick, SRN_ERR);

    g_return_val_if_fail(sui, SRN_ERR);
    chat = sui->chat;

    g_return_val_if_fail(SRAIN_IS_CHAT(chat), SRN_ERR);

    list = srain_chat_get_user_list(chat);

    if ((res = srain_user_list_add(list, nick, type)) == 0){
        comp = srain_chat_get_entry_completion(chat);
        srain_entry_completion_add_keyword(comp, nick, KEYWORD_NORMAL);
    };

    return res;
}

int sui_rm_user(SuiSession *sui, const char *nick){
    int res;
    SrainChat *chat;
    SrainUserList *list;
    SrainEntryCompletion *comp;

    g_return_val_if_fail(nick, SRN_ERR);

    g_return_val_if_fail(sui, SRN_ERR);
    chat = sui->chat;

    g_return_val_if_fail(SRAIN_IS_CHAT(chat), SRN_ERR);

    list = srain_chat_get_user_list(chat);

    if ((res = srain_user_list_rm(list, nick)) == 0){
        comp = srain_chat_get_entry_completion(chat);
        srain_entry_completion_rm_keyword(comp, nick);
    }

    return res;
}

int sui_ren_user(SuiSession *sui, const char *old_nick, const char *new_nick,
        UserType type){
    int ret;
    SrainChat *chat;
    SrainUserList *list;
    SrainEntryCompletion *comp;

    g_return_val_if_fail(old_nick, SRN_ERR);
    g_return_val_if_fail(new_nick, SRN_ERR);

    g_return_val_if_fail(sui, SRN_ERR);
    chat = sui->chat;

    g_return_val_if_fail(SRAIN_IS_CHAT(chat), SRN_ERR);

    /* Your nick changed */
    if (strcmp(old_nick, srain_chat_get_nick(chat)) == 0){
        srain_chat_set_nick(chat, new_nick);
    }

    list = srain_chat_get_user_list(chat);

    ret = srain_user_list_rename(list, old_nick, new_nick, type);

    if (ret == SRN_OK){
        comp = srain_chat_get_entry_completion(chat);
        srain_entry_completion_add_keyword(comp, old_nick, KEYWORD_NORMAL);
        srain_entry_completion_rm_keyword(comp, new_nick);
    }

    return ret;
}

void sui_set_topic(SuiSession *sui, const char *topic){
    SrainChat *chat;

    g_return_if_fail(sui);
    chat = sui->chat;

    g_return_if_fail(SRAIN_IS_CHAT(chat));

    srain_chat_set_topic(chat, topic);
}

void sui_message_set_ctx(SuiMessage *smsg, void *ctx){
    g_return_if_fail(smsg);

    smsg->ctx = ctx;
}

void *sui_message_get_ctx(SuiMessage *smsg){
    g_return_val_if_fail(smsg, NULL);

    return smsg->ctx;
}

void sui_message_append_message(SuiSession *sui, SuiMessage *smsg, const char *msg){
    SrainChat *chat;

    g_return_if_fail(sui);
    g_return_if_fail(smsg);
    g_return_if_fail(msg);

    chat = sui->chat;
    g_return_if_fail(SRAIN_IS_CHAT(chat));

    srain_msg_append_msg(smsg, msg);

    if (SRAIN_IS_RECV_MSG(smsg)){
        srain_window_stack_sidebar_update(srain_win, chat,
                gtk_label_get_text(SRAIN_RECV_MSG(smsg)->nick_label), msg);
    }
    else if (SRAIN_IS_SEND_MSG(smsg)) {
        srain_window_stack_sidebar_update(srain_win, chat, _("You"), msg);
    } else {
        WARN_FR("Append message is not available for message %p", smsg);
    }
}

void sui_message_append_image(SuiMessage *smsg, const char *url){
    SrainImageFlag flag;
    SuiSession *sui;

    g_return_if_fail(smsg);
    g_return_if_fail(url);
    g_return_if_fail(sui_message_get_ctx(smsg));

    sui = sui_message_get_ctx(smsg);
    flag = SRAIN_IMAGE_ENLARGE | SRAIN_IMAGE_SPININER;

    if (sui->prefs->preview_image){
        flag |= SRAIN_IMAGE_AUTOLOAD;
    } else {
    }


    srain_msg_append_image(smsg, url, flag);
}

void sui_message_mentioned(SuiMessage *smsg){
    g_return_if_fail(smsg);

    srain_msg_list_highlight_message(smsg);
}

void sui_message_set_time(SuiMessage *smsg, time_t time){
    srain_msg_set_time(smsg, time);
}

/**
 * @brief sui_message_noitfy Send a desktop notification, if windows is active,
 *        notification will not be sent.
 *
 * @param smsg
 */
void sui_message_notify(SuiMessage *smsg){
    const char *title;
    const char *msg;
    const char *icon;
    SuiSession *sui;

    g_return_if_fail(smsg);

    if (srain_window_is_active(srain_win)){
        return;
    }

    sui = sui_message_get_ctx(smsg);
    if (sui && !sui->prefs->notify){
        return;
    }

    title = NULL;
    msg = gtk_label_get_text(smsg->msg_label);
    icon = "srain";

    if (SRAIN_IS_RECV_MSG(smsg)){
        title = gtk_label_get_text(SRAIN_RECV_MSG(smsg)->nick_label);
    }
    else if (SRAIN_IS_SYS_MSG(smsg)){
        SrainSysMsg *ssmsg = SRAIN_SYS_MSG(smsg);
        if (ssmsg->type == SYS_MSG_ERROR){
            title = _("Error");
            icon = "srain-red";
        }
        else if (ssmsg->type == SYS_MSG_ACTION){
            title = _("Action");
        }
    }

    g_return_if_fail(title);

    snotify_notify(title, msg, icon);
    srain_window_tray_icon_stress(srain_win, 1);
}

void sui_add_completion(SuiSession *sui, const char *keyword){
    SrainChat *chat;
    SrainEntryCompletion *comp;

    g_return_if_fail(sui);
    g_return_if_fail(keyword);

    chat = sui->chat;
    g_return_if_fail(SRAIN_IS_CHAT(chat));

    comp = srain_chat_get_entry_completion(chat);

    srain_entry_completion_add_keyword(comp, keyword, KEYWORD_NORMAL);
}

void sui_rm_completion(SuiSession *sui, const char *keyword){
    SrainChat *chat;
    SrainEntryCompletion *comp;

    g_return_if_fail(sui);
    g_return_if_fail(keyword);

    chat = sui->chat;
    g_return_if_fail(SRAIN_IS_CHAT(chat));

    comp = srain_chat_get_entry_completion(chat);

    srain_entry_completion_rm_keyword(comp, keyword);
}

void sui_message_box(const char *title, const char *msg){
    GtkMessageDialog *dia;

    if (!is_app_run){
        gtk_init(0, NULL);
    }

    dia = GTK_MESSAGE_DIALOG(
            gtk_message_dialog_new(GTK_WINDOW(srain_win),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                NULL
                )
            );

    gtk_window_set_title(GTK_WINDOW(dia), title);
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dia), msg);

    /* Without this, message dialog cannot be displayed on the center of screen */
    sui_proc_pending_event();

    gtk_dialog_run(GTK_DIALOG(dia));
    gtk_widget_destroy(GTK_WIDGET(dia));
}
