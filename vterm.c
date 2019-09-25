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
rb_vterm_initialize(VALUE self, VALUE rows, VALUE cols);
static VALUE
rb_vterm_obtain_screen(VALUE self);
void
Init_vterm(void);


typedef struct {
    VTerm *vt;
    VTermScreen *vtscreen;
} vterm_data_t;

static const rb_data_type_t rb_vterm_type = {
    "vterm",
    {NULL, rb_vterm_free, NULL},
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
    free(vt_data);
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
rb_vterm_obtain_screen(VALUE self)
{
    vterm_data_t *vt_data = (vterm_data_t*)DATA_PTR(self);

    TypedData_Get_Struct(self, vterm_data_t, &rb_vterm_type, vt_data);
    vt_data->vtscreen = vterm_obtain_screen(vt_data->vt);

    return Qnil;
}

static VALUE vterm;

void
Init_vterm(void)
{
    vterm = rb_define_module("VTerm");
    rb_define_alloc_func(vterm, rb_vterm_alloc);
    rb_define_method(vterm, "initialize", rb_vterm_initialize, 2);
    rb_define_module_function(vterm, "obtain_screen", rb_vterm_obtain_screen, 0);
}
