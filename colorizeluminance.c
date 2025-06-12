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
 * Credit to Øvind Kolas (pippin) for major GEGL contributions
 * Colorize Luminance 2022 Beaver 
 */

/*June 2023 recreation of GEGL Graph. If you paste this syntax in Gimp's GEGL graph you can test this filter without installing it.

shadows-highlights
levels 
bloom strength=0
gegl:svg-luminancetoalpha aux=[  color-overlay value=#aaff9d   ]
 color-overlay value=#aaff9d 
gaussian-blur std-dev-x=0.5 std-dev-y=0.5 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (shadow, _("Adjust exposure of highlights"), 0.0)
    description (_("Adjust exposure of highlights"))
    value_range (-100.0, 0.0)


property_double (strength, _("Glow Strength"), 55.0)
    description (_("Bloom's Glow strength"))
    value_range (0.0, 1000)
    ui_range    (0.0, 300.0)

property_double (levels, _("Control light"), 0.5)
    description (_("Higher values make the light dampen, lower values make the light more intense."))
    value_range    (0.1, 1.0)
    ui_range    (0.1, 1.0)

property_double (gaussian, _("Gaussian blur for Glow Effect"), 6.0)
   description (_("Blur that applies to the whole filter. Try it with the union composite mode on a Gimp layer above."))
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
  GeglNode *input, *output, *bloom, *lumen, *color, *gaussian, *shadow, *levels;

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

   levels = gegl_node_new_child (gegl,
                                  "operation", "gegl:levels",
                                  NULL);

   shadow = gegl_node_new_child (gegl,
                                  "operation", "gegl:shadows-highlights", "highlights", 0.0,
                                  NULL);


  gegl_node_link_many (input, shadow, levels, bloom, lumen, color, gaussian, output, NULL);


  gegl_operation_meta_redirect (operation, "shadow", shadow, "highlights");
  gegl_operation_meta_redirect (operation, "strength", bloom, "strength");
  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-y");
  gegl_operation_meta_redirect (operation, "levels", levels, "in-high");
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",               "lb:colorizeluminance",
    "title",              _("Extract and Recolor Luma"),
    "categories",         "Lighting",
    "reference-hash",     "ffhaaa1kfk11ffacxx3gb10b073c9",
    "license",            "GPL3+",
    "description", _("GEGL Extracts the brightest pixels, recolors and blurs  them. This filter is meant to be fused with Gimp's blend modes, such as Addition mid opacity. If the color is white the Luminance and LCH Lightness blend mode will make it glow. Experiment with various blend modes and see what happens."),
    "gimp:menu-path", "<Image>/Filters/Light and Shadow",
    "gimp:menu-label", _("Extract and recolor brightness channel..."),
    NULL);
}

#endif
