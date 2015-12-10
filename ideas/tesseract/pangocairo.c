#include <stdio.h>
#include <pango/pangocairo.h>
#include <cairo-svg.h>

void rendertext(cairo_t *cr) {
	PangoLayout *layout;
	PangoFontDescription *desc;

	cairo_translate(cr, 10, 20);
	layout = pango_cairo_create_layout(cr);

	pango_layout_set_text(layout, "Hello World!", -1);
	desc = pango_font_description_from_string("Sans Bold 12");
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
	pango_cairo_update_layout(cr, layout);
	pango_cairo_show_layout(cr, layout);

	g_object_unref(layout);
}

int main(int argc, char* argv[]) {
	cairo_t *cr;
	cairo_surface_t *surface;

	surface = cairo_svg_surface_create("/tmp/svgfile.svg", 390, 60);
	cr = cairo_create(surface);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	rendertext(cr);

	cairo_surface_destroy(surface);
	cairo_destroy(cr);

	return 0;
}
