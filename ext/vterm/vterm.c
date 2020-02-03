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
rb_vterm_get_size(VALUE self);
static VALUE
rb_vterm_screen_initialize(VALUE self);
static VALUE
rb_vterm_obtain_screen(VALUE self);
static VALUE
rb_vterm_set_utf8(VALUE self, VALUE is_utf8);
static VALUE
rb_vterm_write(VALUE self, VALUE bytes);
static VALUE
rb_vterm_screen_reset(VALUE self, VALUE hard);
static VALUE
rb_vterm_screen_flush(VALUE self);
static VALUE
generate_color_object(VTermColor color);
static VALUE
rb_vterm_screen_cell_at(VALUE self, VALUE row, VALUE col);
void
Init_vterm(void);

static VALUE cVTerm;
static VALUE cVTermScreen;
static VALUE sVTermScreenCell;
static VALUE sVTermScreenCellAttrs;
static VALUE sVTermColorRGB;
static VALUE sVTermColorIndexed;

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
    vt_screen = rb_funcall(cVTermScreen, screen_new_id, 0);

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
rb_vterm_write(VALUE self, VALUE bytes)
{
    vterm_data_t *vt_data;
    char *str;

    vt_data = (vterm_data_t*)DATA_PTR(self);
    str = StringValueCStr(bytes);
    vterm_input_write(vt_data->vt, str, strlen(str));

    return Qnil;
}

static VALUE
rb_vterm_get_size(VALUE self)
{
    int rows, cols;
    vterm_data_t *vt_data;
    VALUE result;

    vt_data = (vterm_data_t*)DATA_PTR(self);
    vterm_get_size(vt_data->vt, &rows, &cols);

    result = rb_ary_new();
    rb_ary_push(result, INT2NUM(rows));
    rb_ary_push(result, INT2NUM(cols));

    return result;
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
rb_vterm_screen_flush(VALUE self)
{
    vterm_screen_data_t *vt_screen_data;

    vt_screen_data = (vterm_screen_data_t*)DATA_PTR(self);
    vterm_screen_flush_damage(vt_screen_data->vtscreen);

    return Qnil;
}

static VALUE
generate_color_object(VTermColor color)
{
    VALUE type;

    if (VTERM_COLOR_IS_DEFAULT_FG(&color)) {
        type = rb_to_symbol(rb_str_new_cstr("fg"));
    } else if (VTERM_COLOR_IS_DEFAULT_BG(&color)) {
        type = rb_to_symbol(rb_str_new_cstr("bg"));
    } else {
        type = Qnil;
    }
    if (VTERM_COLOR_IS_RGB(&color)) {
        return rb_struct_new(
            sVTermColorRGB,
            type,
            INT2NUM(color.rgb.red),
            INT2NUM(color.rgb.green),
            INT2NUM(color.rgb.blue),
            0
        );
    } else if (VTERM_COLOR_IS_INDEXED(&color)) {
        return rb_struct_new(
            sVTermColorIndexed,
            type,
            INT2NUM(color.indexed.idx),
            0
        );
    } else {
        return Qnil;
    }
}

static VALUE
rb_vterm_screen_cell_at(VALUE self, VALUE row, VALUE col)
{
    vterm_screen_data_t *vt_screen_data;
    VTermPos pos;
    VTermScreenCell cell;
    int result;
    VALUE rb_cell;
    VALUE rb_attrs;
    VALUE chars;
    VALUE fg;
    VALUE bg;

    vt_screen_data = (vterm_screen_data_t*)DATA_PTR(self);

    pos.row = NUM2INT(row);
    pos.col = NUM2INT(col);
    result = vterm_screen_get_cell(vt_screen_data->vtscreen, pos, &cell);
    rb_attrs = rb_struct_new(
        sVTermScreenCellAttrs,
        INT2NUM(cell.attrs.bold),
        INT2NUM(cell.attrs.underline),
        INT2NUM(cell.attrs.italic),
        INT2NUM(cell.attrs.blink),
        INT2NUM(cell.attrs.reverse),
        INT2NUM(cell.attrs.strike),
        INT2NUM(cell.attrs.font),
        INT2NUM(cell.attrs.dwl),
        INT2NUM(cell.attrs.dhl),
        0
    );
    fg = generate_color_object(cell.fg);
    bg = generate_color_object(cell.bg);
    chars = rb_external_str_new_cstr("");
    for (int i = 0; i < cell.width; i++) {
        if (cell.chars[i] == 0xFFFFFFFF) {
            // The previous sell has full-width character
            chars = Qnil;
            break;
        } else if (cell.chars[i] == 0x00000000) {
            // NULL termination
            break;
        } else {
            rb_str_concat(chars, INT2NUM(cell.chars[i]));
        }
    }
    rb_cell = rb_struct_new(
        sVTermScreenCell,
        chars,
        rb_attrs,
        fg,
        bg,
        0
    );

    return rb_cell;
}

void
Init_vterm(void)
{
    cVTerm = rb_define_class("VTerm", rb_cObject);
    rb_define_alloc_func(cVTerm, rb_vterm_alloc);
    rb_define_method(cVTerm, "initialize", rb_vterm_initialize, 2);
    rb_define_method(cVTerm, "obtain_screen", rb_vterm_obtain_screen, 0);
    rb_define_method(cVTerm, "set_utf8", rb_vterm_set_utf8, 1);
    rb_define_method(cVTerm, "write", rb_vterm_write, 1);
    rb_define_method(cVTerm, "size", rb_vterm_get_size, 0);

    cVTermScreen = rb_define_class_under(cVTerm, "Screen", rb_cObject);
    rb_define_alloc_func(cVTerm, rb_vterm_screen_alloc);
    rb_define_method(cVTermScreen, "initialize", rb_vterm_screen_initialize, 0);
    rb_define_method(cVTermScreen, "reset", rb_vterm_screen_reset, 1);
    rb_define_method(cVTermScreen, "flush", rb_vterm_screen_flush, 0);
    rb_define_method(cVTermScreen, "cell_at", rb_vterm_screen_cell_at, 2);

    sVTermScreenCell = rb_struct_define_under(cVTermScreen, "Cell", "chars", "attrs", "fg", "bg", NULL);
    sVTermScreenCellAttrs = rb_struct_define_under(cVTermScreen, "CellAttrs", "bold", "underline", "italic", "blink", "reverse", "strike", "font", "dwl", "dhl", NULL);
    sVTermColorRGB = rb_struct_define_under(cVTerm, "ColorRGB", "type", "red", "green", "blue", NULL);
    sVTermColorIndexed = rb_struct_define_under(cVTerm, "ColorIndexed", "type", "index", NULL);
}
