/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * Colorize Luminance 2022 Beaver 
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (shadow, _("Decrease Luminance"), 0.0)
    description (_("Adjust exposure of highlights"))
    value_range (-100.0, 0.0)


property_double (strength, _("Strength of Glow"), 55.0)
    description (_("Glow strength"))
    value_range (0.0, G_MAXDOUBLE)
    ui_range    (0.0, 100.0)

property_double (levels, _("Lower makes opacity higher. Higher makes Transparent"), 1.0)
    description (_("Input luminance level to become white"))
    ui_range    (0.3, 2.0)

property_double (gaussian, _("Gaussian blur to create Glow Effect"), 6.0)
   description (_("Blur that applies to the whole filter"))
   value_range (0.0, 95.0)
   ui_range    (0.24, 95.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")

property_color (value, _("Color"), "#ff9dff")
    description (_("The color to paint over the input"))


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     colorizeluminance
#define GEGL_OP_C_SOURCE colorizeluminance.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *bloom, *lumen, *it, *color, *gaussian, *shadow, *levels;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

   lumen = gegl_node_new_child (gegl,
                                  "operation", "gegl:svg-luminancetoalpha",
                                  NULL);

   bloom = gegl_node_new_child (gegl,
                                  "operation", "gegl:bloom",
                                  NULL);

   color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

   gaussian = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);

   it = gegl_node_new_child (gegl,
                                  "operation", "gegl:invert-transparency",
                                  NULL);

   levels = gegl_node_new_child (gegl,
                                  "operation", "gegl:levels",
                                  NULL);

   shadow = gegl_node_new_child (gegl,
                                  "operation", "gegl:shadows-highlights",
                                  NULL);





  gegl_node_link_many (input, shadow, levels, bloom, lumen, color, gaussian, output, NULL);
  gegl_node_link (input, NULL);
  gegl_node_connect_from (lumen, "aux", color, "output");




  gegl_operation_meta_redirect (operation, "strength", bloom, "strength");
  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-y");
  gegl_operation_meta_redirect (operation, "levels", levels, "in-high");
  gegl_operation_meta_redirect (operation, "shadow", shadow, "highlights");



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",               "gegl:colorizeluminance",
    "title",              _("Extract and Recolor Luminance"),
    "categories",         "render",
    "position-dependent", "true",
    "reference-hash",     "ffhaaa1kfk11ffacxx3gb10b073c9",
    "reference-hashB",    "b1fbcf3h009155h2a00640b0a0a8791",
    "license",            "GPL3+",
    "description", _("It is recommended that you use the Addition blend mode with mid opacity. If the color is white the Luminance and LCH Lightness blend mode will make it glow well. Extract and Colorize Luminance channel'"),
    NULL);
}

#endif
