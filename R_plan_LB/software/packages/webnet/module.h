/*
 * File      : module.h
 * This file is part of RT-Thread RTOS/WebNet Server
 * COPYRIGHT (C) 2011, Shanghai Real-Thread Technology Co., Ltd
 *
 * All rights reserved.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-08-02     Bernard      the first version
 * 2011-08-06     Bernard      add webnet_alias_set declaration
 * 2012-06-25     Bernard      add SSI and Upload module
 */

#ifndef __WEBNET_MODULE_H__
#define __WEBNET_MODULE_H__

#include "session.h"

/* initialization event */
#define WEBNET_EVENT_INIT			(1 << 0)
/* map uri request to physical url */
#define WEBNET_EVENT_URI_PHYSICAL	(1 << 1)
/* uri request */
#define WEBNET_EVENT_URI_POST		(1 << 2)
/* header of response */
#define WEBNET_EVENT_RSP_HEADER		(1 << 3)
/* file content of response */
#define WEBNET_EVENT_RSP_FILE		(1 << 4)


/* continue other modules */
#define WEBNET_MODULE_CONTINUE		0
/* this session is finished */
#define WEBNET_MODULE_FINISHED		1

int webnet_module_handle_event(struct webnet_session* session, int event);
int webnet_module_system_dofile(struct webnet_session* session);
int webnet_module_handle_uri(struct webnet_session* session);

/* module APIs */
void webnet_asp_add_var(const char* name, void (*handler)(struct webnet_session* session));
void webnet_auth_set(const char* path, const char* username_password);
void webnet_cgi_set_root(const char* root);
void webnet_cgi_register(const char* name, void (*handler)(struct webnet_session* session));
void webnet_alias_set(char* old_path, char* new_path);

/* module function pre-declaration */
int webnet_module_alias(struct webnet_session* sesion, int event);
int webnet_module_auth(struct webnet_session* session, int event);
int webnet_module_asp(struct webnet_session* session, int event);
int webnet_module_cgi(struct webnet_session* session, int event);
int webnet_module_dirindex(struct webnet_session* session, int event);
int webnet_module_log(struct webnet_session* session, int event);
int webnet_module_lua(struct webnet_session* sesion, int event);
int webnet_module_ssl(struct webnet_session* sesion, int event);
int webnet_module_ssi(struct webnet_session* session, int event);
int webnet_module_dav(struct webnet_session* session, int event);
int webnet_module_dmr(struct webnet_session* session, int event);

/* upload module */
struct webnet_module_upload_entry
{
    const char* url;

    int (*upload_open) (struct webnet_session* session);
    int (*upload_close)(struct webnet_session* session);
    int (*upload_write)(struct webnet_session* session, const void* data, rt_size_t length);
    int (*upload_done) (struct webnet_session* session);
};
int webnet_module_upload(struct webnet_session* session, int event);
void webnet_upload_add(const struct webnet_module_upload_entry* entry);

const char* webnet_upload_get_filename(struct webnet_session* session);
const char* webnet_upload_get_content_type(struct webnet_session* session);
const char* webnet_upload_get_nameentry(struct webnet_session* session, const char* name);
const void* webnet_upload_get_userdata(struct webnet_session* session);

int webnet_upload_file_open(struct webnet_session* session);
int webnet_upload_file_close(struct webnet_session* session);
int webnet_upload_file_write(struct webnet_session* session, const void* data, rt_size_t length);

#endif
