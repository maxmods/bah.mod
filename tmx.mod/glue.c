/*
Copyright (c) 2015, Bruce A Henderson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "tmx.h"

int bmx_tmx_map_width(tmx_map * map) {
	return map->width;
}

int bmx_tmx_map_height(tmx_map * map) {
	return map->height;
}

int bmx_tmx_map_tile_width(tmx_map * map) {
	return map->tile_width;
}

int bmx_tmx_map_tile_height(tmx_map * map) {
	return map->tile_height;
}

int bmx_tmx_map_backgroundcolor(tmx_map * map) {
	return map->backgroundcolor;
}

int bmx_tmx_map_orient(tmx_map * map) {
	return map->orient;
}

int bmx_tmx_map_renderorder(tmx_map * map) {
	return map->renderorder;
}

tmx_tileset * bmx_tmx_map_ts_head(tmx_map * map) {
	return map->ts_head;
}

tmx_layer * bmx_tmx_map_ly_head(tmx_map * map) {
	return map->ly_head;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int bmx_tmx_tileset_firstgid(tmx_tileset * ts) {
	return ts->firstgid;
}
	
char * bmx_tmx_tileset_name(tmx_tileset * ts) {
	return ts->name;
}

int bmx_tmx_tileset_tile_width(tmx_tileset * ts) {
	return ts->tile_width;
}

int bmx_tmx_tileset_tile_height(tmx_tileset * ts) {
	return ts->tile_height;
}

int bmx_tmx_tileset_spacing(tmx_tileset * ts) {
	return ts->spacing;
}

int bmx_tmx_tileset_margin(tmx_tileset * ts) {
	return ts->margin;
}

int bmx_tmx_tileset_x_offset(tmx_tileset * ts) {
	return ts->x_offset;
}

int bmx_tmx_tileset_y_offset(tmx_tileset * ts) {
	return ts->y_offset;
}

tmx_image * bmx_tmx_tileset_image(tmx_tileset * ts) {
	return ts->image;
}

tmx_property * bmx_tmx_tileset_properties(tmx_tileset * ts) {
	return ts->properties;
}

tmx_tile * bmx_tmx_tileset_tiles(tmx_tileset * ts) {
	return ts->tiles;
}

int bmx_tmx_tileset_hasNext(tmx_tileset * ts) {
	return ts->next != NULL;
}

tmx_tileset * bmx_tmx_tileset_next(tmx_tileset * ts) {
	return ts->next;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

char * bmx_tmx_layer_name(tmx_layer * layer) {
	return layer->name;
}

int bmx_tmx_layer_color(tmx_layer * layer) {
	return layer->color;
}

double bmx_tmx_layer_opacity(tmx_layer * layer) {
	return layer->opacity;
}

int bmx_tmx_layer_visible(tmx_layer * layer) {
	return layer->visible;
}

int bmx_tmx_layer_x_offset(tmx_layer * layer) {
	return layer->x_offset;
}

int bmx_tmx_layer_y_offset(tmx_layer * layer) {
	return layer->y_offset;
}

int bmx_tmx_layer_type(tmx_layer * layer) {
	return layer->type;
}

int32_t * bmx_tmx_layer_gids(tmx_layer * layer) {
	return layer->content.gids;
}

tmx_property * bmx_tmx_layer_properties(tmx_layer * layer) {
	return layer->properties;
}

tmx_object * bmx_tmx_layer_objects(tmx_layer * layer) {
	return layer->content.head;
}

int bmx_tmx_layer_hasNext(tmx_layer * layer) {
	return layer->next != NULL;
}

tmx_layer * bmx_tmx_layer_next(tmx_layer * layer) {
	return layer->next;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

char * bmx_tmx_property_name(tmx_property * prop) {
	return prop->name;
}

char * bmx_tmx_property_value(tmx_property * prop) {
	return prop->value;
}

int bmx_tmx_property_hasNext(tmx_property * prop) {
	return prop->next != NULL;
}

tmx_property * bmx_tmx_property_next(tmx_property * prop) {
	return prop->next;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

char * bmx_tmx_object_name(tmx_object * obj) {
	return obj->name;
}

int bmx_tmx_object_shape(tmx_object * obj) {
	return obj->shape;
}

double bmx_tmx_object_x(tmx_object * obj) {
	return obj->x;
}

double bmx_tmx_object_y(tmx_object * obj) {
	return obj->y;
}

double bmx_tmx_object_width(tmx_object * obj) {
	return obj->width;
}

double bmx_tmx_object_height(tmx_object * obj) {
	return obj->height;
}

int bmx_tmx_object_gid(tmx_object * obj) {
	return obj->gid;
}

double ** bmx_tmx_object_points(tmx_object * obj) {
	return obj->points;
}

int bmx_tmx_object_points_len(tmx_object * obj) {
	return obj->points_len;
}

int bmx_tmx_object_visible(tmx_object * obj) {
	return obj->visible;
}

double bmx_tmx_object_rotation(tmx_object * obj) {
	return obj->rotation;
}

tmx_property* bmx_tmx_object_properties(tmx_object * obj) {
	return obj->properties;
}

int bmx_tmx_object_hasNext(tmx_object * obj) {
	return obj->next != NULL;
}

tmx_object * bmx_tmx_object_next(tmx_object * obj) {
	return obj->next;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

char * bmx_tmx_image_source(tmx_image * image) {
	return image->source;
}

int bmx_tmx_image_trans(tmx_image * image) {
	return image->trans;
}

int bmx_tmx_image_uses_trans(tmx_image * image) {
	return image->uses_trans;
}

int bmx_tmx_image_width(tmx_image * image) {
	return image->width;
}

int bmx_tmx_image_height(tmx_image * image) {
	return image->height;
}

void * bmx_tmx_image_resource_image(tmx_image * image) {
	return image->resource_image;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int bmx_tmx_tile_id(tmx_tile * tile) {
	return tile->id;
}

tmx_image * bmx_tmx_tile_image(tmx_tile * tile) {
	return tile->image;
}

tmx_property * bmx_tmx_tile_properties(tmx_tile * tile) {
	return tile->properties;
}

int bmx_tmx_tile_hasNext(tmx_tile * tile) {
	return tile->next != NULL;
}

tmx_tile * bmx_tmx_tile_next(tmx_tile * tile) {
	return tile->next;
}

