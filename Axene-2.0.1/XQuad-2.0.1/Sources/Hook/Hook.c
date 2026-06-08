/*
** Hook.c for Xclamation, XQuad and XAllWrite in Hook/
** Methods for the Hook class
**
** Copyright (C) 1994-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Tue Dec 27 18:05:45 1994 Stéphane Boisson
** Last update Thu Jan  6 23:44:13 2000 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "Hook.h"


static void *constructor();
static void destructor();
static void *copy();
static void callback ___PROTO((c_Hook *this, int reason, void *new));
static boolean merge ___PROTO((c_Hook *target, c_Hook *source));
static boolean addCallback ___PROTO((c_Hook *this,
				     XcHookCallbackProc cb_func,
				     void *cb_data));
static boolean addOneCallback ___PROTO((c_Hook *this,
					XcHookCallbackProc cb_func,
					void *cb_data));
static void removeCallback ___PROTO((c_Hook *this,
				     XcHookCallbackProc cb_func,
				     void *cb_data));
static boolean changeHook ___PROTO((c_Hook *old, c_Hook *new,
				    XcHookCallbackProc cb_func,
				    void *cb_data));
static boolean grow_buffer ___PROTO((c_Hook *this, unsigned long inc));
static unsigned long getUsage ___PROTO((c_Hook *this, void *cb_data));
static unsigned long getClientData ___PROTO((c_Hook *this, void ***pptr));

sf_Hook fc_Hook =
{
 constructor, destructor, copy,
 getUsage,
 callback, merge,
 addCallback, removeCallback,
 changeHook,
 getClientData,
 addOneCallback 
};


static ItemHandle *i_handle = NULL;
static long i_references = 0;


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(data, increment)
void *data;
unsigned long increment;
{
 c_Hook *this;

 Xc_HISTORY(("constructor"));
 Xc_ASSERT(increment > 0);

 if(i_handle == NULL)
  i_handle = Xc_createItem("Hook base", 16, sizeof(c_Hook));
 if(i_handle == NULL) return NULL;

 if((this = Xc_new(i_handle)) == NULL) return NULL;
 this->f = &fc_Hook;
 i_references++;

 this->server_data = data;
 this->increment = increment;
 this->count = this->max_count = 0;
 this->clients = NULL;
 
 Xc_TRACE(("constructor end (%lx)", (long)this));
 return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Hook *this;
{
 Xc_HISTORY(("destructor(%lx)", (long)this));
 Xc_ASSERT(i_references > 0);

#if 0		/* not usable with inheritance */
#ifdef NTRACE
 Xc_ASSERT(this->count == 0);
#else
 if(this->count != 0)
 {
  hook_client_t *client;
  unsigned long i;

  for(client = this->clients, i = this->count; i > 0; i--, client++)
   Xc_TRACE(("%d times: func=%lx data=%lx", client->count,
	     (long)client->cb_func, (long)client->cb_data));
  Xc_BREAK(("Hook still referenced"));
 }
#endif
#endif

 if(this->clients != NULL) Xc_free(this->clients);

 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_TRACE(("destroy base"));
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 }
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- **
** Copier                                                            **
** ----------------------------------------------------------------- */
static void *copy(this)
c_Hook *this;
{
 Xc_HISTORY(("copy: non implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** callback - Callback clients for a server message                  ** 
** ----------------------------------------------------------------- */
static void callback(this, reason, new)
c_Hook *this;
int reason;
void *new;
{
 hook_client_t *client;
 unsigned long i;

 Xc_HISTORY(("callback"));

 for(client = this->clients, i = this->count; i > 0; i--, client++)
  client->cb_func(client->cb_data, reason, this->server_data, new);
 this->server_data = new;
 Xc_TRACE(("done"));
}


/* ----------------------------------------------------------------- ** 
** merge - Merge two Hook clients list                               ** 
** ----------------------------------------------------------------- */
static boolean merge(target, source)
c_Hook *target;
c_Hook *source;
{
 Xc_HISTORY(("merge"));
 Xc_ASSERT(source != target);

 if(source->count == 0) return TRUE;
 if(((target->count + source->count) >= target->max_count) &&
    !grow_buffer(target, target->count + source->count - target->max_count))
  return FALSE;
 memcpy(target->clients + target->count,
	source->clients,
	source->count * sizeof(hook_client_t));
 target->count += source->count;

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addCallback - Add a callback for a client                         ** 
** ----------------------------------------------------------------- */
static boolean addCallback(this, cb_func, cb_data)
c_Hook *this;
XcHookCallbackProc cb_func;
void *cb_data;
{
 hook_client_t *client;
 unsigned long i;

 Xc_HISTORY(("addCallback($%lx, $%lx, $%lx)",
	     (long)this, (long)cb_func, (long)cb_data));

 for(i = this->count, client = this->clients; i-- > 0; client++)
  if((client->cb_func == cb_func) && (client->cb_data == cb_data))
  {
   client->count++;
   Xc_TRACE(("ref #%d", client->count));
   return TRUE;
  }

 if((this->count == this->max_count) && !grow_buffer(this, this->increment))
  return FALSE;
 client = this->clients + this->count++;
 client->cb_func = cb_func;
 client->cb_data = cb_data;
 client->count = 1;

 Xc_TRACE(("added"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addOneCallback - Add only one callback per client                 ** 
** ----------------------------------------------------------------- */
static boolean addOneCallback(this, cb_func, cb_data)
c_Hook *this;
XcHookCallbackProc cb_func;
void *cb_data;
{
 hook_client_t *client;
 unsigned long i;

 Xc_HISTORY(("addOneCallback($%lx, $%lx, $%lx)",
	     (long)this, (long)cb_func, (long)cb_data));

 for(i = this->count, client = this->clients; i-- > 0; client++)
  if((client->cb_func == cb_func) && (client->cb_data == cb_data))
  {
   Xc_TRACE(("ref already at #%d", client->count));
   return TRUE;
  }

 if((this->count == this->max_count) && !grow_buffer(this, this->increment))
  return FALSE;
 client = this->clients + this->count++;
 client->cb_func = cb_func;
 client->cb_data = cb_data;
 client->count = 1;

 Xc_TRACE(("added"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** removeCallback - Remove a callback for a client                   ** 
** ----------------------------------------------------------------- */
static void removeCallback(this, cb_func, cb_data)
c_Hook *this;
XcHookCallbackProc cb_func;
void *cb_data;
{
 hook_client_t *client;
 unsigned long i;

 Xc_HISTORY(("removeCallback($%lx, $%lx, $%lx)",
	     (long)this, (long)cb_func, (long)cb_data));

 for(i = this->count, client = this->clients; i-- > 0; client++)
 {
  if((client->cb_func == cb_func) && (client->cb_data == cb_data))
  {
   if(client->count-- == 1)
   {
    if(i > 0) memmove(client, client + 1, i * sizeof(hook_client_t));
    this->count--;
    Xc_TRACE(("removed"));
    return;
   }
   else
   {
    Xc_TRACE(("ref #%d", client->count));
    return;
   }
  }
 }
  
 Xc_BREAK(("not found"));
 return;
}


/* ----------------------------------------------------------------- ** 
** changeHook - Switch a client between two Hooks                    ** 
** ----------------------------------------------------------------- */
static boolean changeHook(old, new, cb_func, cb_data)
c_Hook *old;
c_Hook *new;
XcHookCallbackProc cb_func;
void *cb_data;
{
 Xc_HISTORY(("changeHook($%lx, $%lx, $%lx, $%lx)",
	     (long)old, (long)new, (long)cb_func, (long)cb_data));

 if((new != NULL) &&
    (addCallback(new, cb_func, cb_data) == FALSE)) return FALSE;
 if(old != NULL) removeCallback(old, cb_func, cb_data);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** grow_buffer - Try to grow client buffer                           ** 
** ----------------------------------------------------------------- */
static boolean grow_buffer(this, inc)
c_Hook *this;
unsigned long inc;
{
 hook_client_t *new;
 unsigned long size;

 Xc_TRACE(("grow_buffer"));

 Xc_mcheck();
 size = this->max_count + inc;
 new = Xc_realloc("Clients", this->clients, size * sizeof(hook_client_t));
 if(new == NULL) return FALSE;
 this->clients = new;
 this->max_count = size;
 Xc_mcheck();
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** getUsage - Get usage for an hook                                  ** 
** ----------------------------------------------------------------- */
static unsigned long getUsage(this, cb_data)
c_Hook *this;
void *cb_data;
{
 hook_client_t *client;
 unsigned long i, count;

 Xc_HISTORY(("getUsage"));

 if(cb_data == NULL) return this->count;

 for(i = this->count, count = 0, client = this->clients; i-- > 0; client++)
  if(client->cb_data == cb_data) count += client->count;
 return count;
}


/* ----------------------------------------------------------------- ** 
** getClientData - Get all the client datas                          ** 
** ----------------------------------------------------------------- */
static unsigned long getClientData(this, pptr)
c_Hook *this;
void ***pptr;
{
 hook_client_t *client;
 unsigned long i;
 void **ptr;

 Xc_HISTORY(("getClientData"));

 if((this->count != 0) &&
    ((*pptr = Xc_malloc("data", this->count * sizeof(void *))) != NULL))
 {
  ptr = *pptr;
  for(i = this->count, client = this->clients; i-- > 0; client++)
   *ptr++ = client->cb_data;
 }
 return this->count;
}
