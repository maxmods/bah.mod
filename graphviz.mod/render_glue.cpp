
extern "C" {

#include "gvplugin.h"

	extern gvplugin_installed_t gvrender_bmxgen_types;
	extern gvplugin_installed_t gvtextlayout_bmxgen_types;
	extern gvplugin_installed_t gvdevice_bmxgen_types;
}

static gvplugin_api_t apis[] = {
    {API_render, &gvrender_bmxgen_types},
    {API_textlayout, &gvtextlayout_bmxgen_types},
    {API_device, &gvdevice_bmxgen_types},
    {(api_t)0, 0},
};

gvplugin_library_t gvplugin_bmx_LTX_library = { "bmx", apis };
