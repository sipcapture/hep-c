#include "hep/hep_io.h"

#include <string.h>

static hep_io_hooks_t hooks;
static void *hooks_user_data;
static int hooks_installed;

void hep_set_io_hooks(const hep_io_hooks_t *new_hooks, void *user_data)
{
	if (new_hooks) {
		hooks = *new_hooks;
		hooks_user_data = user_data;
		hooks_installed = 1;
	} else {
		memset(&hooks, 0, sizeof(hooks));
		hooks_user_data = NULL;
		hooks_installed = 0;
	}
}

const hep_io_hooks_t *hep_get_io_hooks(void **user_data_out)
{
	if (!hooks_installed)
		return NULL;
	if (user_data_out)
		*user_data_out = hooks_user_data;
	return &hooks;
}
