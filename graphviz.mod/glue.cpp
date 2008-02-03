
extern "C" {

#include <gvc.h>
#include "gvplugin_render.h"
#include "gvplugin_textlayout.h"
#include "gvplugin_device.h"
#include <gvcint.h>
#include "gvcproc.h"
#include <blitz.h>

	int colorxlate(char *str, gvcolor_t * color, color_type_t target_type);
	void gvjobs_delete(GVC_t * gvc);
	void graph_cleanup(graph_t *g);

	void _bah_graphviz_TGVGraphviz_beginJob(GVJ_t * job);
	void _bah_graphviz_TGVGraphviz_endJob(GVJ_t * job);
	void _bah_graphviz_TGVGraphviz_textPara(void * handle, double x, double y, textpara_t * para);
	void _bah_graphviz_TGVGraphviz_ellipse(void * handle, pointf * A, int filled);
	void _bah_graphviz_TGVGraphviz_polygon(void * handle, pointf * A, int n, int filled);
	void _bah_graphviz_TGVGraphviz_bezier(void * handle, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled);
	void _bah_graphviz_TGVGraphviz_polyline(void * handle, pointf * A, int n);
	char * _bah_graphviz_TGVGraphviz_textLayout(GVCOMMON_t *common, textpara_t * para);
	void _bah_graphviz_TGVGraphviz_deviceInitialize(GVJ_t * job);
	void _bah_graphviz_TGVGraphviz_deviceFinalize(GVJ_t * job);

	GVC_t * bmx_gvc_new();
	graph_t * bmx_agmemread(char * text);

	void bmx_setpencolor(GVJ_t * job, double * r, double * g, double * b, double * a);
	void bmx_setfillcolor(GVJ_t * job, double * r, double * g, double * b, double * a);
	void bmx_setstyle(GVJ_t * job, int * pen, int * fill, double * penwidth);

	char * bmx_para_str(textpara_t * para);
	char * bmx_para_fontname(textpara_t * para);
	void bmx_para_fontmetrics(textpara_t * para, double * size, double * width, double * height);
	char bmx_para_just(textpara_t * para);
	
	int bmx_color_type(gvcolor_t * color);
	void bmx_para_setlayout(textpara_t * para, void * font);
	void bmx_para_setmetrics(textpara_t * para, double size, double width, double height);
	void * bmx_para_layout(textpara_t * para);
	
	void bmx_gvj_size(GVJ_t * job, unsigned int * width, unsigned int * height);
	
	//void bmx_gvj_setView(GVJ_t * job, double width, double height);
	void bmx_gvj_setSize(GVJ_t * job, unsigned int width, unsigned int height);
	
	void bmx_job_refresh(GVJ_t * job);
	void bmx_gv_setdpi(GVJ_t * job, int x, int y);
	
	void bmx_gvParseArgs(GVC_t * gvc, char * how);
	
	void bmx_job_setPointer(GVJ_t * job, int x, int y);
	void bmx_job_mouseDown(GVJ_t * job, int x, int y, int button);
	void bmx_job_mouseUp(GVJ_t * job, int x, int y, int button);
	void bmx_job_mouseScroll(GVJ_t * job, int x, int y, int direction);
	
	void bmx_gvj_zoom(GVJ_t * job, double * zoom);
	void bmx_gvj_setZoom(GVJ_t * job, double zoom);
	void bmx_gvj_pageOffset(GVJ_t * job, double * x, double * y);
	void bmx_gvj_focus(GVJ_t * job, double * x, double * y);
	void bmx_gvj_viewSize(GVJ_t * job, double * width, double * height);
	
	GVC_t * bmx_job_getGVC(GVJ_t * job);
	BBObject * bmx_job_getWindow(GVJ_t * job);
	void bmx_job_setWindow(GVJ_t * job, void * win);
	
	char * bmx_job_active_tooltip(GVJ_t * job);
	int bmx_color_translate(char * txt, int * r, int * g, int * b, int * a);
	
	bool bmx_job_hasSelection(GVJ_t * job);
	int bmx_job_attrCount(GVJ_t * job, int type, int count);
	//char * bmx_job_attrsName(GVJ_t * job, int type, int count, int i);
	char * bmx_job_attrsValue(GVJ_t * job, int type, int i);
	
	void bmx_gvjobs_delete(GVC_t * gvc);
	Agraph_t * bmx_gvevent_layout(GVJ_t * job, char *layout, char * text);
	void bmx_gvFreeLayout(GVC_t * gvc, graph_t * g);
	
	void bmx_graph_bgcolor(GVJ_t * job, int * r, int * g, int * b, int * a);
	
	void * bmx_job_checkCurrent(GVJ_t *job, void * obj, gv_argvlist_t * typelist, gv_argvlist_t * attrlist);
	int bmx_obj_attrCount(gv_argvlist_t * list, int count);
	char * bmx_obj_attrsValue(gv_argvlist_t * list, int i);
	
	int bmx_agsafesetHTML(void *obj, char *name, char *value, char* dflt);

}


GVC_t * bmx_gvc_new() {
	return gvContext();
}

graph_t * bmx_agmemread(char * text) {
	return agmemread(text);
}

void bmx_set_color(gvcolor_t color, double * r, double * g, double * b, double * a) {
	*r = color.u.RGBA[0];
	*g = color.u.RGBA[1];
	*b = color.u.RGBA[2];
	*a = color.u.RGBA[3];	
}

void bmx_setpencolor(GVJ_t * job, double * r, double * g, double * b, double * a) {
	
	bmx_set_color(job->obj->pencolor, r, g, b, a);

}

void bmx_setfillcolor(GVJ_t * job, double * r, double * g, double * b, double * a) {
	
	bmx_set_color(job->obj->fillcolor, r, g, b, a);

}

void bmx_setstyle(GVJ_t * job, int * pen, int * fill, double * penwidth) {
	*pen = (int)job->obj->pen;
	*fill = (int)job->obj->fill;
	*penwidth = job->obj->penwidth;
}

char * bmx_para_str(textpara_t * para) {
	return para->str;
}

char * bmx_para_fontname(textpara_t * para) {
	return para->fontname;
}

void bmx_para_fontmetrics(textpara_t * para, double * size, double * width, double * height) {
	*size = para->fontsize;
	*width = para->width;
	*height = para->height;
}

char bmx_para_just(textpara_t * para) {
	return para->just;
}

int bmx_color_type(gvcolor_t * color) {
	return color->type;
}

void bmx_para_setlayout(textpara_t * para, void * font) {
	para->layout = font;
}

void bmx_para_setmetrics(textpara_t * para, double size, double width, double height) {
	para->fontsize = size;
	para->width = width;
	para->height = height;
	// we don't use these...
	para->layout = NULL;
	para->xshow = NULL;
	para->free_layout = NULL;
}

void * bmx_para_layout(textpara_t * para) {
	return para->layout;
}

void bmx_gvj_size(GVJ_t * job, unsigned int * width, unsigned int * height) {
	*width = job->width;
	*height = job->height;
}

void bmx_gvj_setSize(GVJ_t * job, unsigned int width, unsigned int height) {
	job->width = width;
	job->height = height;
}

void bmx_gvj_focus(GVJ_t * job, double * x, double * y) {
	*x = job->focus.x;
	*y = job->focus.y;
}

void bmx_textLayout(GVCOMMON_t *common, textpara_t * para, char **fontpath) {
	char * fp = _bah_graphviz_TGVGraphviz_textLayout(common, para);
	*fontpath = fp;
}

void bmx_job_refresh(GVJ_t * job) {

	job->gvc->common.viewNum = 0; // need this to be reset... otherwise we get duplicates drawn.
	
	job->external_surface = TRUE;
	
	(job->callbacks->motion)(job, job->pointer);

	(job->callbacks->refresh)(job);		
}

void bmx_gv_setdpi(GVJ_t * job, int x, int y) {
	job->device_dpi.x = x;
	job->device_dpi.y = y;
	job->device_sets_dpi = true;	
}

void bmx_gvParseArgs(GVC_t * gvc, char * how) {
	char * args[] = { "neato", "-Tbmx", "../graphs/undirected/ER.dot" };
//GVC_t* g = gvContext();
printf("A\n");fflush(stdout);
	gvParseArgs(gvc, sizeof(args)/sizeof(char*), args);
printf("B\n");fflush(stdout);
printf("B1\n");fflush(stdout);
}


void bmx_job_setPointer(GVJ_t * job, int x, int y) {
	job->pointer.x = x;
	job->pointer.y = y;
}

void bmx_job_mouseDown(GVJ_t * job, int x, int y, int button) {
	pointf pointer;
	pointer.x = x;
	pointer.y = y;
	(job->callbacks->button_press)(job, button, pointer);
}

void bmx_job_mouseUp(GVJ_t * job, int x, int y, int button) {
	pointf pointer;
	pointer.x = x;
	pointer.y = y;
	(job->callbacks->button_release)(job, button, pointer);
}

void bmx_job_mouseScroll(GVJ_t * job, int x, int y, int direction) {
	if (direction != 0) {
		pointf pointer;
		pointer.x = x;
		pointer.y = y;
		if (direction < 0) {
			(job->callbacks->button_press)(job, 5, pointer);
		} else {
			(job->callbacks->button_press)(job, 4, pointer);
		}
	}
}

void bmx_gvj_zoom(GVJ_t * job, double * zoom) {
	*zoom = job->zoom;
}

void bmx_gvj_pageOffset(GVJ_t * job, double * x, double * y) {
	*x = job->pageOffset.x;
	*y = job->pageOffset.y;
}

void bmx_gvj_viewSize(GVJ_t * job, double * width, double * height) {
	*width = job->view.x;
	*height = job->view.y;
}

GVC_t * bmx_job_getGVC(GVJ_t * job) {
	return job->gvc;
}

BBObject * bmx_job_getWindow(GVJ_t * job) {
	if (job->window == NULL) {
		return &bbNullObject;
	}
	return (BBObject *)job->window;
}

void bmx_job_setWindow(GVJ_t * job, void * win) {
	job->window = win;
}


void bmx_render_textPara(GVJ_t * job, pointf p, textpara_t * para) {
	_bah_graphviz_TGVGraphviz_textPara(job->window, p.x, p.y, para);
}

void bmx_render_ellipse(GVJ_t * job, pointf * A, int filled) {
	_bah_graphviz_TGVGraphviz_ellipse(job->window, A, filled);
}

void bmx_render_polygon(GVJ_t * job, pointf * A, int n, int filled) {
	_bah_graphviz_TGVGraphviz_polygon(job->window, A, n, filled);
}

void bmx_render_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled) {
	_bah_graphviz_TGVGraphviz_bezier(job->window, A, n, arrow_at_start, arrow_at_end, filled);
}

void bmx_render_polyline(GVJ_t * job, pointf * A, int n) {
	_bah_graphviz_TGVGraphviz_polyline(job->window, A, n);
}

int bmx_color_translate(char * txt, int * r, int * g, int * b, int * a) {
	gvcolor_t color;
	int ret = colorxlate(txt, &color, RGBA_BYTE);
	if (!ret) {
		*r = (int)color.u.rgba[0];
		*g = (int)color.u.rgba[1];
		*b = (int)color.u.rgba[2];
		*a = (int)color.u.rgba[3];	
	}
	return !ret;
}

char * bmx_job_active_tooltip(GVJ_t * job) {
	return job->active_tooltip;
}

bool bmx_job_hasSelection(GVJ_t * job) {
	if (job->selected_obj) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int bmx_job_attrCount(GVJ_t * job, int type, int count) {

	if (job->selected_obj) {
		if (type == 1) {
			return job->selected_obj_type_name.argc / count;
		} else {
			return job->selected_obj_attributes.argc / count;
		}
	}
	
	return 0;
}

char * bmx_job_attrsValue(GVJ_t * job, int type, int i) {

	if (job->selected_obj) {
		if (type == 1) {
			return job->selected_obj_type_name.argv[i];
		} else {
			return job->selected_obj_attributes.argv[i];
		}
	}

	return NULL;
}

void bmx_gvj_setZoom(GVJ_t * job, double zoom) {
	job->zoom *= zoom;
	job->needs_refresh = TRUE;
}

void bmx_gvjobs_delete(GVC_t * gvc) {
	gvjobs_delete(gvc);
}

//void bmx_gvevent_layout(GVJ_t * job, char * layout) {
//	gvLayout(job->gvc, job->gvc->g, layout);
//}

Agraph_t * bmx_gvevent_layout(GVJ_t * job, char *layout, char * text)
{
printf("A\n");fflush(stdout);
    GVC_t *gvc;
    Agraph_t *g = NULL;
//    gvlayout_engine_t *gvle;

    gvc = job->gvc;
	g = agmemread(text);
printf("B\n");fflush(stdout);

    if (gvc->g) {
printf("C\n");fflush(stdout);

	graph_cleanup(gvc->g);
printf("C1\n");fflush(stdout);
	//agclose(gvc->g);
    }
    gvc->g = g;
    GD_gvc(g) = gvc;
printf("D\n");fflush(stdout);
    gvLayout(gvc, g, layout);
printf("E\n");fflush(stdout);
    job->selected_obj = NULL;
    job->current_obj = NULL;
    job->needs_refresh = 1;
	job->zoom = 1.0;
	
	return g;
}

void bmx_gvFreeLayout(GVC_t * gvc, graph_t * g) {
	gvjobs_delete(gvc);
	gvFreeLayout(gvc, g);
}

void bmx_graph_bgcolor(GVJ_t * job, int * r, int * g, int * b, int * a) {
	Agsym_t *as = agfindattr(job->gvc->g->root, "bgcolor");
	
	if (!as) {
		*r = *g = *b = *a = 255;
	} else {
		bmx_color_translate(agxget(job->gvc->g->root, as->index), r, g, b, a);
	}
}

void bmx_graph_state(graph_t *g, gv_argvlist_t * typelist, gv_argvlist_t * attrlist)
{
    int i, j;
    Agsym_t *a;
    gv_argvlist_t *list;

    list = typelist;
    j = 0;
    if (g == g->root) {
	if (g->kind && AGFLAG_DIRECTED) 
            gv_argvlist_set_item(list, j++, "digraph");
	else
            gv_argvlist_set_item(list, j++, "graph");
    }
    else {
        gv_argvlist_set_item(list, j++, "subgraph");
    }
    gv_argvlist_set_item(list, j++, g->name);
    list->argc = j;

    list = attrlist;
    for (i = 0, j = 0; i < dtsize(g->univ->globattr->dict); i++) {
        a = g->univ->globattr->list[i];
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, agxget(g, a->index));
        gv_argvlist_set_item(list, j++, (char*)GVATTR_STRING);
    }
    list->argc = j;
}

void bmx_node_state(node_t *n, gv_argvlist_t * typelist, gv_argvlist_t * attrlist)
{
    int i, j;
    Agsym_t *a;
    Agraph_t *g;
    gv_argvlist_t *list;

    list = typelist;
    j = 0;
    gv_argvlist_set_item(list, j++, "node");
    gv_argvlist_set_item(list, j++, n->name);
    list->argc = j;

    list = attrlist;
    g = n -> graph -> root;
    for (i = 0, j = 0; i < dtsize(g->univ->nodeattr->dict); i++) {
        a = g->univ->nodeattr->list[i];
        gv_argvlist_set_item(list, j++, a->name);
        gv_argvlist_set_item(list, j++, agxget(n, a->index));
    }
    list->argc = j;
}

void bmx_edge_state(edge_t *e, gv_argvlist_t * typelist, gv_argvlist_t * attrlist)
{
    int i, j;
    Agsym_t *a;
    Agraph_t *g;
    gv_argvlist_t *nlist, *alist;

    nlist = typelist;

    /* only tail, head, and key are strictly identifying properties,
     * but we commonly alse use edge kind (e.g. "->") and tailport,headport
     * in edge names */
    j = 0;
    gv_argvlist_set_item(nlist, j++, "edge");
    gv_argvlist_set_item(nlist, j++, e->tail->name);
    j++; /* skip tailport slot for now */
	if (e->tail->graph->kind && AGFLAG_DIRECTED) {
		gv_argvlist_set_item(nlist, j++, "->");
	} else {
		gv_argvlist_set_item(nlist, j++, "--");
	}
    gv_argvlist_set_item(nlist, j++, e->head->name);
    j++; /* skip headport slot for now */
    j++; /* skip key slot for now */
    nlist->argc = j;

    alist = attrlist;
    g = e -> head -> graph -> root;
    for (i = 0, j = 0; i < dtsize(g->univ->edgeattr->dict); i++) {
        a = g->univ->edgeattr->list[i];

	/* tailport and headport can be shown as part of the name, but they
	 * are not identifying properties of the edge so we 
	 * also list them as modifyable attributes. */
        if (strcmp(a->name,"tailport") == 0)
	    gv_argvlist_set_item(nlist, 2, agxget(e, a->index));
	else if (strcmp(a->name,"headport") == 0)
	    gv_argvlist_set_item(nlist, 5, agxget(e, a->index));

	/* key is strictly an identifying property to distinguish multiple
	 * edges between the same node pair.   Its non-writable, so
	 * no need to list it as an attribute as well. */
	else if (strcmp(a->name,"key") == 0) {
	    gv_argvlist_set_item(nlist, 6, agxget(e, a->index));
	    continue;
	}

        gv_argvlist_set_item(alist, j++, a->name);
        gv_argvlist_set_item(alist, j++, agxget(e, a->index));
    }
    alist->argc = j;
}


void * bmx_job_checkCurrent(GVJ_t *job, void * obj, gv_argvlist_t * typelist, gv_argvlist_t * attrlist) {
	void * currentObj = job->current_obj;
	
	if (obj == currentObj) {
		return obj;
	} else {
		if (currentObj) {
			switch (agobjkind(currentObj)) {
			case AGGRAPH:
				bmx_graph_state((graph_t*)currentObj, typelist, attrlist);
				break;
			case AGNODE:
				bmx_node_state((node_t*)currentObj, typelist, attrlist);
				break;
			case AGEDGE:
				bmx_edge_state((edge_t*)currentObj, typelist, attrlist);
				break;
			}
			
			return currentObj;
		}
	}
	
	return obj;
}


int bmx_obj_attrCount(gv_argvlist_t * list, int count) {
	return list->argc / count;
}

char * bmx_obj_attrsValue(gv_argvlist_t * list, int i) {
	return list->argv[i];
}



gvrender_engine_t bmxgen_engine = {
    _bah_graphviz_TGVGraphviz_beginJob,				/* bmxgen_begin_job */
    _bah_graphviz_TGVGraphviz_endJob,				/* bmxgen_end_job */
    0,
    0,
    0,				/* bmxgen_begin_layer */
    0,				/* bmxgen_end_layer */
    0,				/* bmxgen_begin_page */
    0,				/* bmxgen_end_page */
    0,				/* bmxgen_begin_cluster */
    0,				/* bmxgen_end_cluster */
    0,				/* bmxgen_begin_nodes */
    0,				/* bmxgen_end_nodes */
    0,				/* bmxgen_begin_edges */
    0,				/* bmxgen_end_edges */
    0,				/* bmxgen_begin_node */
    0,				/* bmxgen_end_node */
    0,				/* bmxgen_begin_edge */
    0,				/* bmxgen_end_edge */
    0,				/* bmxgen_begin_anchor */
    0,				/* bmxgen_end_anchor */
    bmx_render_textPara,
    0,				/* bmxgen_resolve_color */
    bmx_render_ellipse,
    bmx_render_polygon,
    bmx_render_bezier,
    bmx_render_polyline,
    0,				/* bmxgen_comment */
    0, /* bmxgen_user shape */
};


gvrender_features_t bmxgen_features = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_DOES_TRANSFORM
	| GVRENDER_X11_EVENTS
	| GVRENDER_Y_GOES_DOWN,	/* flags */
    0,				/* default margin - points */
    0.,                         /* default pad - graph units */
    {320.,200.},                    /* default page width, height - points */
    {1.,1.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    "bmx",			/* device */
    0,			/* gvloadimage target for usershapes */
};

gvplugin_installed_t gvrender_bmxgen_types[] = {
    {0, "bmx", 10, &bmxgen_engine, &bmxgen_features},
    {0, NULL, 0, NULL, NULL}
};


gvtextlayout_engine_t bmx_textlayout_engine = {
    bmx_textLayout,
};

gvplugin_installed_t gvtextlayout_bmxgen_types[] = {
    {0, "textlayout", 2, &bmx_textlayout_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};

gvdevice_engine_t bmx_device_engine = {
    _bah_graphviz_TGVGraphviz_deviceInitialize,
    _bah_graphviz_TGVGraphviz_deviceFinalize,
};

gvplugin_installed_t gvdevice_bmxgen_types[] = {
    {0, "bmx", 0, &bmx_device_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};

int bmx_agsafesetHTML(void *obj, char *name, char *value, char* dflt) {
  int rv;
  char* hvalue = agstrdup_html(value);
  rv = agsafeset(obj, name, hvalue, dflt);
  agstrfree(hvalue);
  return(rv);
}
