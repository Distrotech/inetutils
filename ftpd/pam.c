#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "extern.h"

#ifdef HAVE_SECURITY_PAM_APPL_H
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#endif

#ifdef WITH_PAM

static int PAM_conv __P ((int num_msg, const struct pam_message **msg,
			  struct pam_response **resp, void *appdata_ptr));
static struct pam_conv PAM_conversation = { &PAM_conv, NULL };

/* PAM authentication, now using the PAM's async feature.  */
pam_handle_t *pamh;
int PAM_accepted;
char *PAM_username;
char *PAM_password;
char *PAM_message;

static int
PAM_conv (int num_msg, const struct pam_message **msg,
	  struct pam_response **resp, void *appdata_ptr)
{
  struct pam_response *repl = NULL;
  int retval, count = 0, replies = 0;
  int size = sizeof(struct pam_response);

#define GET_MEM \
        if (!(repl = realloc (repl, size))) \
                return PAM_CONV_ERR; \
        size += sizeof (struct pam_response)

#define COPY_STRING(s) (s) ? strdup (s) : NULL

  (void) appdata_ptr;
  retval = PAM_SUCCESS;
  for (count = 0; count < num_msg; count++)
    {
      int savemsg = 0;

      switch (msg[count]->msg_style)
	{
	case PAM_PROMPT_ECHO_ON:
	  GET_MEM;
	  repl[replies].resp_retcode = PAM_SUCCESS;
	  repl[replies].resp = COPY_STRING (PAM_username);
	  replies++;
	  break;
	case PAM_PROMPT_ECHO_OFF:
	  GET_MEM;
	  if (PAM_password == 0)
	    {
	      savemsg = 1;
	      retval = PAM_CONV_AGAIN;
	    }
	  else
	    {
	      repl[replies].resp_retcode = PAM_SUCCESS;
	      repl[replies].resp = COPY_STRING (PAM_password);
	      replies++;
	    }
	  break;
	case PAM_TEXT_INFO:
	  savemsg = 1;
	  break;
	case PAM_ERROR_MSG:
	default:
	  /* Must be an error of some sort... */
	  reply (530, "%s", msg[count]->msg);
	  retval = PAM_CONV_ERR;
	}

      if (savemsg)
	{
	  char *sp;

	  if (PAM_message) /* XXX: make sure we split newlines correctly */
	    {
	      lreply (331, "%s", PAM_message);
	      free (PAM_message);
	    }
	  PAM_message = COPY_STRING (msg[count]->msg);

	  /* Remove trailing `: ' */
	  sp = PAM_message + strlen (PAM_message);
	  while (sp > PAM_message && strchr (" \t\n:", *--sp))
	    *sp = '\0';
	}

      /* In case of error, drop responses and return */
      if (retval)
	{
	  _pam_drop_reply (repl, replies);
	  return retval;
	}
    }
  if (repl)
    *resp = repl;
  return PAM_SUCCESS;
}

static int
pam_doit (void)
{
  char *user;
  int error;

  error = pam_authenticate (pamh, 0);
  if (error == PAM_CONV_AGAIN || error == PAM_INCOMPLETE)
    {
      /* Avoid overly terse passwd messages */
      if (PAM_message && !strcasecmp (PAM_message, "password"))
	{
	  free (PAM_message);
	  PAM_message = 0;
	}
      if (PAM_message == 0)
	{
	  reply (331, "Password required for %s.", PAM_username);
	}
      else
	{
	  reply (331, "%s", PAM_message);
	  free (PAM_message);
	  PAM_message = 0;
	}
      return 1;
    }
  if (error == PAM_SUCCESS) /* Alright, we got it */
    {
      error = pam_acct_mgmt (pamh, 0);
      if (error == PAM_SUCCESS)
	error = pam_setcred (pamh, PAM_ESTABLISH_CRED);
      if (error == PAM_SUCCESS)
	error = pam_get_item (pamh, PAM_USER, (const void **) &user);
      if (error == PAM_SUCCESS)
	{
	  if (strcmp (user, "ftp") == 0)
	    {
	      guest = 1;
	    }
	  pw = sgetpwnam (user);
	}
      if (error == PAM_SUCCESS && pw)
	PAM_accepted = 1;
    }
  pam_end(pamh, error);
  pamh = 0;

  return (error == PAM_SUCCESS);
}

static void
authentication_setup (const char *username)
{
  int error;

  if (pamh != 0)
    {
      pam_end (pamh, PAM_ABORT);
      pamh = 0;
    }

  if (PAM_username)
    free (PAM_username);
  PAM_username = strdup (username);
  PAM_password = 0;
  PAM_message  = 0;
  PAM_accepted = 0;

  error = pam_start ("ftp", PAM_username, &PAM_conversation, &pamh);
  if (error == PAM_SUCCESS)
    error = pam_set_item (pamh, PAM_RHOST, remotehost);
  if (error != PAM_SUCCESS)
    {
      reply (550, "Authentication failure");
      pam_end (pamh, error);
      pamh = 0;
    }

  if (pamh && !pam_doit ())
    reply(550, "Authentication failure");
}

#endif /* WITH_PAM */
