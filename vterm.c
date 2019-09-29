#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "vterm.h"
#include "ruby.h"


static VALUE
rb_vterm_alloc(VALUE klass);
static void
rb_vterm_free(void *ptr);
static VALUE
rb_vterm_screen_alloc(VALUE klass);
static void
rb_vterm_screen_free(void *ptr);
static VALUE
rb_vterm_initialize(VALUE self, VALUE rows, VALUE cols);
static VALUE
rb_vterm_screen_initialize(VALUE self);
static VALUE
rb_vterm_obtain_screen(VALUE self);
static VALUE
rb_vterm_set_utf8(VALUE self, VALUE is_utf8);
static VALUE
rb_vterm_input_write(VALUE self, VALUE bytes);
static VALUE
rb_vterm_screen_reset(VALUE self, VALUE hard);
static VALUE
rb_vterm_screen_flush_damage(VALUE self, VALUE hard);
void
Init_vterm(void);

static VALUE vterm;
static VALUE vterm_screen;

typedef struct {
    VTerm *vt;
} vterm_data_t;

typedef struct {
    VTermScreen *vtscreen;
} vterm_screen_data_t;

static const rb_data_type_t rb_vterm_type = {
    "vterm",
    {NULL, rb_vterm_free, NULL},
    NULL, NULL,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static const rb_data_type_t rb_vterm_screen_type = {
    "vterm_screen",
    {NULL, rb_vterm_screen_free, NULL},
    NULL, NULL,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE
rb_vterm_alloc(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &rb_vterm_type, NULL);
}

static void
rb_vterm_free(void *ptr)
{
    vterm_data_t *vt_data = ptr;
    vterm_free(vt_data->vt);
    free(vt_data);
}

static VALUE
rb_vterm_screen_alloc(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &rb_vterm_screen_type, NULL);
}

static void
rb_vterm_screen_free(void *ptr)
{
    vterm_screen_data_t *vt_screen_data = ptr;
    free(vt_screen_data);
}

static VALUE
rb_vterm_initialize(VALUE self, VALUE rows, VALUE cols)
{
    vterm_data_t *vt_data;

    vt_data = malloc(sizeof(vterm_data_t));
    DATA_PTR(self) = vt_data;

    vt_data->vt = vterm_new(NUM2INT(rows), NUM2INT(cols));

    return Qnil;
}

static VALUE
rb_vterm_screen_initialize(VALUE self)
{
    vterm_screen_data_t *vt_screen_data;

    vt_screen_data = malloc(sizeof(vterm_screen_data_t));
    DATA_PTR(self) = vt_screen_data;

    return Qnil;
}

static VALUE
rb_vterm_obtain_screen(VALUE self)
{
    vterm_data_t *vt_data;
    vterm_screen_data_t *vt_screen_data;
    ID screen_new_id;
    VALUE vt_screen;

    vt_data = (vterm_data_t*)DATA_PTR(self);
    screen_new_id = rb_intern("new");
    vt_screen = rb_funcall(vterm_screen, screen_new_id, 0);

    vt_screen_data = malloc(sizeof(vterm_screen_data_t));
    vt_screen_data->vtscreen = vterm_obtain_screen(vt_data->vt);
    DATA_PTR(vt_screen) = vt_screen_data;

    return vt_screen;
}

static VALUE
rb_vterm_set_utf8(VALUE self, VALUE is_utf8)
{
    vterm_data_t *vt_data;

    vt_data = (vterm_data_t*)DATA_PTR(self);
    if (rb_equal(is_utf8, Qfalse) || rb_equal(is_utf8, Qnil)) {
        vterm_set_utf8(vt_data->vt, 0);
    } else {
        vterm_set_utf8(vt_data->vt, 1);
    }

    return Qnil;
}

static VALUE
rb_vterm_input_write(VALUE self, VALUE bytes)
{
    vterm_data_t *vt_data;
    char *str;

    vt_data = (vterm_data_t*)DATA_PTR(self);
    str = StringValueCStr(bytes);
    vterm_input_write(vt_data->vt, str, sizeof(str));

    return Qnil;
}

static VALUE
rb_vterm_screen_reset(VALUE self, VALUE hard)
{
    vterm_screen_data_t *vt_screen_data;

    vt_screen_data = (vterm_screen_data_t*)DATA_PTR(self);
    if (rb_equal(hard, Qfalse) || rb_equal(hard, Qnil)) {
        vterm_screen_reset(vt_screen_data->vtscreen, 0);
    } else {
        vterm_screen_reset(vt_screen_data->vtscreen, 1);
    }

    return Qnil;
}

static VALUE
rb_vterm_screen_flush_damage(VALUE self, VALUE hard)
{
    vterm_screen_data_t *vt_screen_data;

    vt_screen_data = (vterm_screen_data_t*)DATA_PTR(self);
    vterm_screen_flush_damage(vt_screen_data->vtscreen);

    return Qnil;
}

void
Init_vterm(void)
{
    vterm = rb_define_class("VTerm", rb_cObject);
    rb_define_alloc_func(vterm, rb_vterm_alloc);
    rb_define_method(vterm, "initialize", rb_vterm_initialize, 2);
    rb_define_method(vterm, "obtain_screen", rb_vterm_obtain_screen, 0);
    rb_define_method(vterm, "set_utf8", rb_vterm_set_utf8, 1);
    rb_define_method(vterm, "input_write", rb_vterm_input_write, 1);

    vterm_screen = rb_define_class_under(vterm, "Screen", rb_cObject);
    rb_define_alloc_func(vterm, rb_vterm_screen_alloc);
    rb_define_method(vterm_screen, "initialize", rb_vterm_screen_initialize, 0);
    rb_define_method(vterm_screen, "reset", rb_vterm_screen_reset, 1);
    rb_define_method(vterm_screen, "flush_damage", rb_vterm_screen_flush_damage, 0);
}
