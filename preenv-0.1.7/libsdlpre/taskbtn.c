#include <glib.h>

#include "config.h"
#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN "TASKBTN"

static gint taskbtn_timer = 0;

static gboolean taskbtn_timeout_cb()
{
	PDL_Minimize();
	taskbtn_timer = 0;
	return FALSE;
}

void TaskBtn_Press()
{
	if (!taskbtn_timer) {
		taskbtn_timer = g_timeout_add_seconds(TASKNAV_BTN_TIME,
			taskbtn_timeout_cb, NULL);
	}
}

void TaskBtn_Release()
{
	if (taskbtn_timer) {
		g_source_remove(taskbtn_timer);
		taskbtn_timer = 0;
	}
}

