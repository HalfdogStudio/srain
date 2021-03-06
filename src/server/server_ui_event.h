#ifndef __UI_EVENT_H
#define __UI_EVENT_H

#include "sui/sui.h"

void server_ui_event_activate(SuiEvent event, const char *params[], int count);
void server_ui_event_connect(SuiEvent event, const char *params[], int count);

void server_ui_event_disconnect(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_send(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_join(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_part(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_query(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_unquery(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_kick(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_invite(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_whois(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_ignore(SuiSession *sui, SuiEvent event, const char *params[], int count);
void server_ui_event_cutover(SuiSession *sui, SuiEvent event, const char *params[], int count);

#endif /* __UI_EVENT_H */
