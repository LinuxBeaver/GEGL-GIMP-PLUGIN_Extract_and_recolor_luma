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
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * Colorize Luminance 2022 Beaver 
 */

/*June 2023 recreation of GEGL Graph. If you paste this syntax in Gimp's GEGL graph you can test this filter without installing it.

id=1 
plus aux=[  ref=1 

shadows-highlights
levels 
bloom strength=40
gegl:svg-luminancetoalpha 
 color-overlay value=#e68ffc
gaussian-blur std-dev-x=0.5 std-dev-y=0.5 

]

*/

#include "config.h"
#include <glib/gi18n-lib.h>


#ifdef GEGL_PROPERTIES


property_boolean (easy_mode, _("Auto Blend with Plus blend mode"), FALSE)
  description    (_("Auto blend with the gegl:plus blend mode at 70% opacity for quality results. Plus is a blend mode exclusive to GEGL which behaves very similar to GIMP's addition blend mode. Both addition and plus are the most recommended blend mode for this filter. The technical reason I used GEGL's Plus instead of GIMPs addition is because I want to make my filters work outside of GIMP and if it used GIMP's addition that would mean no other app but GIMP could use it "))


property_double (shadow, _("Adjust exposure of highlights"), -90.0)
    description (_("Adjust exposure of highlights"))
    value_range (-100.0, 0.0)


property_double (strength, _("Glow Strength"), 55.0)
    description (_("Bloom's Glow strength"))
    value_range (0.0, 1000)
    ui_range    (0.0, 300.0)

property_double (levels, _("Control light (reverse sliders) "), 0.7)
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

/*starred nodes go inside typedef struct */

typedef struct
{
 GeglNode *input;
 GeglNode *bloom;
 GeglNode *lumen; 
 GeglNode *color;
 GeglNode *gaussian;
 GeglNode *levels;
 GeglNode *shadow;
 GeglNode *plus;
 GeglNode *opacity;
 GeglNode *replacelock;
 GeglNode *output;
}State;

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);
/*example of a embedded color 

  GeglColor *embeddedcolor = gegl_color_new ("#000000");*/

  State *state = o->user_data = g_malloc0 (sizeof (State));

/*new child node list is here, this is where starred nodes get defined*/
  state->input    = gegl_node_get_input_proxy (gegl, "input");
  state->output   = gegl_node_get_output_proxy (gegl, "output");

   state->lumen = gegl_node_new_child (gegl,
                                  "operation", "gegl:svg-luminancetoalpha",
                                  NULL);

   state->bloom = gegl_node_new_child (gegl,
                                  "operation", "gegl:bloom",
                                  NULL);

   state->color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

   state->gaussian = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);

   state->levels = gegl_node_new_child (gegl,
                                  "operation", "gegl:levels",
                                  NULL);

   state->shadow = gegl_node_new_child (gegl,
                                  "operation", "gegl:shadows-highlights",
                                  NULL);

   state->plus = gegl_node_new_child (gegl,
                                  "operation", "gegl:plus", "srgb", FALSE,
                                  NULL);

   state->opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 0.70,
                                  NULL);

   state->replacelock = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-in",
                                  NULL);

/*meta redirect property to new child orders go here

 gegl_operation_meta_redirect (operation, "propertyname", state->newchildname,  "originalpropertyname");
*/
}

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  if (o->easy_mode) 
  gegl_node_link_many (state->input, state->replacelock, state->output,  NULL);
 gegl_node_connect (state->replacelock, "aux", state->plus, "output");
  gegl_node_link_many (state->input, state->plus,  NULL);
  gegl_node_link_many ( state->input, state->shadow, state->levels, state->bloom, state->lumen, state->color, state->gaussian,  state->opacity, NULL);
 gegl_node_connect (state->plus, "aux", state->opacity, "output");

  if (!o->easy_mode) 
  gegl_node_link_many ( state->input, state->shadow, state->levels, state->bloom, state->lumen, state->color, state->gaussian, state->output, NULL);

/*optional connect from and too is here
  gegl_node_connect (state->blendmode, "aux", state->lastnodeinlist, "output"); */
  gegl_operation_meta_redirect (operation, "shadow", state->shadow, "highlights");
  gegl_operation_meta_redirect (operation, "strength", state->bloom, "strength");
  gegl_operation_meta_redirect (operation, "value", state->color, "value");
  gegl_operation_meta_redirect (operation, "gaussian", state->gaussian, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian", state->gaussian, "std-dev-y");
  gegl_operation_meta_redirect (operation, "levels", state->levels, "in-high");

}



static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",               "lb:colorizeluminance",
    "title",              _("Extract and Recolor Luma (Glow)"),
    "categories",         "Lighting",
    "reference-hash",     "ffhaaa1kfk11ffacxx3gb10b073c9",
    "license",            "GPL3+",
    "description", _("Somewhat similar to the Bloom filter except you can choose the color. Extract the brightest pixels, recolors them and blur it  them. This filter is meant to be fused with GIMP's blend modes, such as plus mid opacity. If the color is white the Luminance and LCH Lightness blend mode will make it glow. Experiment with various blend modes and see what happens"),
    "gimp:menu-path", "<Image>/Filters/Light and Shadow",
    "gimp:menu-label", _("Extract and recolor luminance channel (Glow)..."),
    NULL);
}

#endif
