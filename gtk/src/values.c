/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * presets.c
 * Copyright (C) John Stebbins 2008 <stebbins@stebbins>
 * 
 * presets.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 */

#include <glib.h>
#include <glib-object.h>
#include <string.h>
#include "values.h"

static void dict_delete_key(gpointer data);
static void dict_delete_value(gpointer data);

GValue*
ghb_value_new(GType gtype)
{
	GValue *gval = g_malloc0(sizeof(GValue));
	g_value_init(gval, gtype);
	return gval;
}

void
ghb_value_free(GValue *gval)
{
	if (gval == NULL) return;
	g_value_unset(gval);
	g_free(gval);
}

GValue*
ghb_value_dup(const GValue *val)
{
	if (val == NULL) return NULL;
	GValue *copy = ghb_value_new(G_VALUE_TYPE(val));
	g_value_copy(val, copy);
	return copy;
}

gint
ghb_value_int(const GValue *val)
{
	if (val == NULL) return 0;
	const GValue *gval;
	GValue xform = {0,};
	if (G_VALUE_TYPE(val) != G_TYPE_INT64)
	{
		g_value_init(&xform, G_TYPE_INT64);
		if (!g_value_transform(val, &xform))
			return 0;
		gval = &xform;
	}
	else
	{
		gval = val;
	}
	return (gint)g_value_get_int64(gval);
}

gint64
ghb_value_int64(const GValue *val)
{
	if (val == NULL) return 0;
	const GValue *gval;
	GValue xform = {0,};
	if (G_VALUE_TYPE(val) != G_TYPE_INT64)
	{
		g_value_init(&xform, G_TYPE_INT64);
		if (!g_value_transform(val, &xform))
			return 0;
		gval = &xform;
	}
	else
	{
		gval = val;
	}
	return g_value_get_int64(gval);
}

gdouble
ghb_value_double(const GValue *val)
{
	if (val == NULL) return 0;
	const GValue *gval;
	GValue xform = {0,};
	if (G_VALUE_TYPE(val) != G_TYPE_DOUBLE)
	{
		g_value_init(&xform, G_TYPE_DOUBLE);
		if (!g_value_transform(val, &xform))
			return 0;
		gval = &xform;
	}
	else
	{
		gval = val;
	}
	return g_value_get_double(gval);
}

gchar*
ghb_value_string(const GValue *val)
{
	if (val == NULL) return 0;
	const GValue *gval;
	GValue xform = {0,};
	if (G_VALUE_TYPE(val) != G_TYPE_STRING)
	{
		g_value_init(&xform, G_TYPE_STRING);
		if (!g_value_transform(val, &xform))
			return NULL;
		gval = &xform;
	}
	else
	{
		gval = val;
	}
	return g_strdup(g_value_get_string(gval));
}

gboolean
ghb_value_boolean(const GValue *val)
{
	if (val == NULL) return FALSE;
	const GValue *gval;
	GValue xform = {0,};
	if (G_VALUE_TYPE(val) != G_TYPE_BOOLEAN)
	{
		g_value_init(&xform, G_TYPE_BOOLEAN);
		if (!g_value_transform(val, &xform))
			return FALSE;
		gval = &xform;
	}
	else
	{
		gval = val;
	}
	return g_value_get_boolean(gval);
}

gint
ghb_value_cmp(const GValue *vala, const GValue *valb)
{
	GType typa;
	GType typb;

	typa = G_VALUE_TYPE(vala);
	typb = G_VALUE_TYPE(valb);
	if (typa == ghb_combodata_get_type()) typa = G_TYPE_STRING;
	if (typb == ghb_combodata_get_type()) typb = G_TYPE_STRING;
	if (typa != typb)
	{
		return 1;
	}
	
	if (typa == G_TYPE_STRING)
	{
		char *stra, *strb;
		gint res;
		stra = ghb_value_string(vala);
		strb = ghb_value_string(valb);
		if (stra == NULL && strb == NULL)
			return 0;
		if (stra == NULL)
		{
			g_free(strb);
			return -1;
		}
		if (strb == NULL)
		{
			g_free(stra);
			return 1;
		}
		res =  strcmp(stra, strb);
		g_free(stra);
		g_free(strb);
		return res;
	}
	else if (typa == G_TYPE_INT64 || typa == G_TYPE_INT || 
			typa == G_TYPE_BOOLEAN)
	{
		return ghb_value_int64(vala) - ghb_value_int64(valb);
	}
	else if (typa == G_TYPE_DOUBLE || typa == G_TYPE_FLOAT)
	{
		return ghb_value_double(vala) - ghb_value_double(valb);
	}
	else if (typa == ghb_array_get_type())
	{
		// Cheating here.  Just assume they are different.
		// Maybe later I'll recurse through these
		return 1;
	}
	else if (typa == ghb_dict_get_type())
	{
		// Cheating here.  Just assume they are different.
		// Maybe later I'll recurse through these
		return 1;
	}
	else
	{
		g_warning("ghb_value_cmp: unrecognized type");
		return 1;
	}
	return 0;
}

GValue*
ghb_string_value(const gchar *str)
{
	static GValue gval = {0,};
	if (!G_IS_VALUE(&gval))
		g_value_init(&gval, G_TYPE_STRING);
	g_value_set_string(&gval, str);
	return &gval;
}

GValue*
ghb_int64_value(gint64 ival)
{
	static GValue gval = {0,};
	if (!G_IS_VALUE(&gval))
		g_value_init(&gval, G_TYPE_INT64);
	g_value_set_int64(&gval, ival);
	return &gval;
}

GValue*
ghb_int_value(gint ival)
{
	static GValue gval = {0,};
	if (!G_IS_VALUE(&gval))
		g_value_init(&gval, G_TYPE_INT64);
	g_value_set_int64(&gval, (gint64)ival);
	return &gval;
}

GValue*
ghb_double_value(gdouble dval)
{
	static GValue gval = {0,};
	if (!G_IS_VALUE(&gval))
		g_value_init(&gval, G_TYPE_DOUBLE);
	g_value_set_double(&gval, dval);
	return &gval;
}

GValue*
ghb_boolean_value(gboolean bval)
{
	static GValue gval = {0,};
	if (!G_IS_VALUE(&gval))
		g_value_init(&gval, G_TYPE_BOOLEAN);
	g_value_set_boolean(&gval, bval);
	return &gval;
}

GValue*
ghb_string_value_new(const gchar *str)
{
	if (str == NULL) str = "";
	GValue *gval = ghb_value_new(G_TYPE_STRING);
	g_value_set_string(gval, str);
	return gval;
}

GValue*
ghb_int64_value_new(gint64 ival)
{
	GValue *gval = ghb_value_new(G_TYPE_INT64);
	g_value_set_int64(gval, ival);
	return gval;
}

GValue*
ghb_int_value_new(gint ival)
{
	GValue *gval = ghb_value_new(G_TYPE_INT64);
	g_value_set_int64(gval, ival);
	return gval;
}

GValue*
ghb_double_value_new(gdouble dval)
{
	GValue *gval = ghb_value_new(G_TYPE_DOUBLE);
	g_value_set_double(gval, dval);
	return gval;
}

GValue*
ghb_boolean_value_new(gboolean bval)
{
	GValue *gval = ghb_value_new(G_TYPE_BOOLEAN);
	g_value_set_boolean(gval, bval);
	return gval;
}

GValue*
ghb_combo_value_new(
	gint index,
	const gchar *option,
	const gchar *shortOpt,
	const gchar *svalue,
	gint ivalue)
{
	GValue *gval = ghb_value_new(ghb_combodata_get_type());
    ghb_value_set_combodata(gval, index, option, shortOpt, svalue, ivalue);
	return gval;
}

GValue*
ghb_dict_value_new()
{
	GHashTable *dict;
	GValue *gval = ghb_value_new(ghb_dict_get_type());
	dict = g_hash_table_new_full(g_str_hash, g_str_equal,
								dict_delete_key, dict_delete_value);
	g_value_take_boxed(gval, dict);
	return gval;
}

GValue*
ghb_array_value_new(guint size)
{
	GValue *gval = ghb_value_new(ghb_array_get_type());
	GArray *array;

	array = g_array_sized_new(FALSE, FALSE, sizeof(GValue*), size);
	g_value_take_boxed(gval, array);
	return gval;
}

void
ghb_array_value_reset(GValue *gval, guint size)
{
	GArray *array;
	g_value_reset(gval);
	array = g_array_sized_new(FALSE, FALSE, sizeof(GValue*), size);
	g_value_take_boxed(gval, array);
}

GValue* 
ghb_date_value_new(GDate *date)
{
	GValue *gval = ghb_value_new(g_date_get_type());
	g_value_set_boxed(gval, date);
	return gval;
}

GValue* 
ghb_rawdata_value_new(ghb_rawdata_t *data)
{
	GValue *gval = ghb_value_new(ghb_rawdata_get_type());
	g_value_take_boxed(gval, data);
	return gval;
}

static gpointer
combodata_copy(gpointer boxed)
{
	const ghb_combodata_t *combodata = (const ghb_combodata_t*)boxed;
	ghb_combodata_t *copy = g_malloc0(sizeof(ghb_combodata_t));
	if (combodata->option)
		copy->option = g_strdup(combodata->option);
	if (combodata->shortOpt)
		copy->shortOpt = g_strdup(combodata->shortOpt);
	if (combodata->svalue)
		copy->svalue = g_strdup(combodata->svalue);

	copy->index = combodata->index;
	copy->ivalue = combodata->ivalue;
	return copy;
}

static void
combodata_free(gpointer boxed)
{
	ghb_combodata_t *combodata = (ghb_combodata_t*)boxed;
	if (combodata->option) 
		g_free(combodata->option);
	if (combodata->shortOpt) 
		g_free(combodata->shortOpt);
	if (combodata->svalue) 
		g_free(combodata->svalue);
	g_free(combodata);
}


static void
xform_combodata_to_string(const GValue *combo, GValue *sval)
{
	const ghb_combodata_t *combodata = g_value_get_boxed(combo);
	g_value_set_string(sval, combodata->shortOpt);
}

static void
xform_combodata_to_int64(const GValue *combo, GValue *ival)
{
	const ghb_combodata_t *combodata = g_value_get_boxed(combo);
	g_value_set_int64(ival, combodata->ivalue);
}

static void
xform_combodata_to_double(const GValue *combo, GValue *dval)
{
	const ghb_combodata_t *combodata = g_value_get_boxed(combo);
	g_value_set_double(dval, (gdouble)combodata->ivalue);
}

GType
ghb_combodata_get_type(void)
{
	static GType type_id = 0;
	if (!type_id)
	{
		type_id = g_boxed_type_register_static(
					g_intern_static_string("GHBCombo"),
					(GBoxedCopyFunc) combodata_copy,
					(GBoxedFreeFunc) combodata_free);
		g_value_register_transform_func(type_id, G_TYPE_STRING, 
									xform_combodata_to_string);
		g_value_register_transform_func(type_id, G_TYPE_INT64, 
									xform_combodata_to_int64);
		g_value_register_transform_func(type_id, G_TYPE_DOUBLE, 
									xform_combodata_to_double);
	}
	return type_id;
}

void
ghb_value_set_combodata(
	GValue *gval, 
	gint index,
	const gchar *option,
	const gchar *shortOpt,
	const gchar *svalue,
	gint ivalue)
{
	ghb_combodata_t combodata;
	combodata.index = index;
	combodata.option = g_strdup(option);
	combodata.shortOpt = g_strdup(shortOpt);
	combodata.svalue = g_strdup(svalue);
	combodata.ivalue = ivalue;
	g_value_set_boxed(gval, &combodata);
}

static gpointer
rawdata_copy(gpointer boxed)
{
	const ghb_rawdata_t *data = (const ghb_rawdata_t*)boxed;
	ghb_rawdata_t *copy = g_malloc(sizeof(ghb_rawdata_t));
	copy->size = data->size;
	if (data->data)
	{
		copy->data = g_malloc(data->size);
		memcpy(copy->data, data->data, data->size);
	}
	else
	{
		copy->data = NULL;
		copy->size = 0;
	}
	return copy;
}

static void
rawdata_free(gpointer boxed)
{
	ghb_rawdata_t *data = (ghb_rawdata_t*)boxed;
	if (data->data) g_free(data->data);
	g_free(data);
}


GType
ghb_rawdata_get_type(void)
{
	static GType type_id = 0;
	if (!type_id)
		type_id = g_boxed_type_register_static(g_intern_static_string("GHBData"),
			(GBoxedCopyFunc) rawdata_copy,
			(GBoxedFreeFunc) rawdata_free);
	return type_id;
}

static void
dict_delete_key(gpointer data)
{
	if (data == NULL)
	{
		g_warning("dict frees null key");
		return;
	}
	g_free(data);
}

static void
dict_delete_value(gpointer data)
{
	GValue *gval = (GValue*)data;
	if (gval == NULL)
	{
		g_warning("dict frees null value");
		return;
	}
	ghb_value_free(gval);
}

static gpointer
dict_copy(gpointer boxed)
{
	GHashTable *dict = (GHashTable*)boxed;
	GHashTable *copy;
	GHashTableIter iter;
	gchar *key;
	GValue *gval;

	copy = g_hash_table_new_full(g_str_hash, g_str_equal,
								dict_delete_key, dict_delete_value);

	g_hash_table_iter_init(&iter, dict);
	while (g_hash_table_iter_next(&iter, (gpointer*)&key, (gpointer*)&gval))
	{
		g_hash_table_insert(copy, g_strdup(key), ghb_value_dup(gval));
	}
	return copy;
}

static void
dict_free(gpointer boxed)
{
	GHashTable *dict = (GHashTable*)boxed;
	g_hash_table_destroy(dict);
}


GType
ghb_dict_get_type(void)
{
	static GType type_id = 0;
	if (!type_id)
		type_id = g_boxed_type_register_static(
						g_intern_static_string("GHBDict"),
						(GBoxedCopyFunc) dict_copy,
						(GBoxedFreeFunc) dict_free);
	return type_id;
}

void
ghb_dict_insert(GValue *gval, gchar *key, GValue *val)
{
	GHashTable *dict = g_value_get_boxed(gval);
	g_hash_table_insert(dict, key, val);
}

void
ghb_dict_iter_init(GHashTableIter *iter, GValue *gval)
{
	GHashTable *dict = g_value_get_boxed(gval);
	g_hash_table_iter_init(iter, dict);
}

GValue*
ghb_dict_lookup(GValue *gval, const gchar *key)
{
	GHashTable *dict = g_value_get_boxed(gval);
	return g_hash_table_lookup(dict, key);
}

gboolean
ghb_dict_remove(GValue *gval, const gchar *key)
{
	GHashTable *dict = g_value_get_boxed(gval);
	return g_hash_table_remove(dict, key);
}

static gpointer
array_copy(gpointer boxed)
{
	const GArray *array = (const GArray*)boxed;
	GArray *copy = g_array_new(FALSE, FALSE, sizeof(GValue*));

	GValue *gval, *gval_copy;
	gint ii;

	for (ii = 0; ii < array->len; ii++)
	{
		gval = g_array_index(array, GValue*, ii);
		if (gval)
		{
			gval_copy = ghb_value_dup(gval);
			g_array_append_val(copy, gval_copy);
		}
	}
	return copy;
}

static void
array_free(gpointer boxed)
{
	GArray *array = (GArray*)boxed;
	GValue *gval;
	gint ii;

	for (ii = 0; ii < array->len; ii++)
	{
		gval = g_array_index(array, GValue*, ii);
		if (gval)
		{
			ghb_value_free(gval);
		}
	}
	g_array_free(array, TRUE);
}


GType
ghb_array_get_type(void)
{
	static GType type_id = 0;
	if (!type_id)
		type_id = g_boxed_type_register_static(
						g_intern_static_string("GHBArray"),
						(GBoxedCopyFunc) array_copy,
						(GBoxedFreeFunc) array_free);
	return type_id;
}

GValue*
ghb_array_get_nth(const GValue *gval, gint ii)
{
	GArray *arr = g_value_get_boxed(gval);
	return g_array_index(arr, GValue*, ii);
}

void
ghb_array_append(GValue *gval, GValue *val)
{
	GArray *arr = g_value_get_boxed(gval);
	// A little nastyness here.  The array pointer
	// can change when the array changes size.  So
	// I must re-box it in the GValue each time.
	arr = g_array_append_val(arr, val);
	memset(gval, 0, sizeof(GValue));
	g_value_init(gval, ghb_array_get_type());
	g_value_take_boxed(gval, arr);
}

void
ghb_array_remove(GValue *gval, guint ii)
{
	GArray *arr = g_value_get_boxed(gval);
	// A little nastyness here.  The array pointer
	// can change when the array changes size.  So
	// I must re-box it in the GValue each time.
	arr = g_array_remove_index(arr, ii);
	memset(gval, 0, sizeof(GValue));
	g_value_init(gval, ghb_array_get_type());
	g_value_take_boxed(gval, arr);
}

gint
ghb_array_len(const GValue *gval)
{
	if (gval == NULL) return 0;
	GArray *arr = g_value_get_boxed(gval);
	return arr->len;
}

