# GEGL Extract and Color Luminance Channel
GEGL Extracts the brightness channel from an image and colorizes and blurs it. This filter was inspired by Gimp/GEGL's "Bloom" and Pixelitor's "Extract channel" filter. It shows that GEGL has the ability to extract channels for non-destructive editing with blend modes.  

#Rules for filter to work
1. Filter requires an alpha channel

2. It is highly recommended you use the blend mode addition. 

 3.Gimp's layer blend modes will support an external glow effect on transparency. GEGL's layer blend modes will NOT support an external glow effect on transparency.

Here is a preview of me using the filter with Gimp's layer blend modes correctly to get an external glow effect.
![image preview](preview_of_EL.png  )
