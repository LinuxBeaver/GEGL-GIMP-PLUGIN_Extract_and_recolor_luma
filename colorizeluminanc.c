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

shadows-highlights
levels 
bloom strength=0
gegl:svg-luminancetoalpha aux=[  color-overlay value=#aaff9d   ]
 color-overlay value=#aaff9d 
gaussian-blur std-dev-x=0.5 std-dev-y=0.5 */




#include "config.h"
#include <glib/gi18n-lib.h>


#ifdef GEGL_PROPERTIES


property_boolean (easy_mode, _("Auto Blend with Add blend mode"), FALSE)
  description    (_("Auto blend with the add blend mode. Add is a blend mode exclusive to GEGL that behaves the same as GIMP's addition blend mode and addition is the most recommended blend mode for this filter"))


property_double (shadow, _("Adjust exposure of highlights"), -100.0)
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


*/

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
 GeglNode *addition;
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

   state->addition = gegl_node_new_child (gegl,
                                  "operation", "gegl:add",
                                  NULL);

   state->opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity", "value", 0.75,
                                  NULL);


/*meta redirect property to new child orders go here

 gegl_operation_meta_redirect (operation, "propertyname", state->newchildname,  "originalpropertyname");
*/


  gegl_operation_meta_redirect (operation, "shadow", state->shadow, "highlights");
  gegl_operation_meta_redirect (operation, "strength", state->bloom, "strength");
  gegl_operation_meta_redirect (operation, "value", state->color, "value");
  gegl_operation_meta_redirect (operation, "gaussian", state->gaussian, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian", state->gaussian, "std-dev-y");
  gegl_operation_meta_redirect (operation, "levels", state->levels, "in-high");
}

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

/*update graph starred nodes go here 

  GeglNode *starrednode = state->defaultsetting; */

/*drop down list settings go here 
state->defaultsetting comes from new child list 

  switch (o->algorithm) {
    case DROPDOWNLIST1: starrednode = state->defaultsetting; break;
    case DROPDOWNLIST2: starrednode = state->altsetting; break;
default: starrednode = state->defaultsetting; 

}
*/ 

/* 
checkbox on and off settings here, state->donothing = gegl:nop

o-> switchcheckbox is a propertyname

  if (o->enablecheckbox) starrednode2 = state->dosomething;
  if (!o->enablerotate) starrednode2 = state->donothing;
*/



  if (o->easy_mode) 
  gegl_node_link_many (state->input, state->addition, state->opacity, state->output, NULL);
  gegl_node_link_many ( state->input, state->shadow, state->levels, state->bloom, state->lumen, state->color, state->gaussian, NULL);
 gegl_node_connect (state->add, "aux", state->gaussian, "output");

  if (!o->easy_mode) 
  gegl_node_link_many ( state->input, state->shadow, state->levels, state->bloom, state->lumen, state->color, state->gaussian, state->output, NULL);


/*optional connect from and too is here
  gegl_node_connect (state->blendmode, "aux", state->lastnodeinlist, "output"); */

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
