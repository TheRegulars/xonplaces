#ifndef GL_CUSTOM_H 
#define GL_CUSTOM_H

typedef enum glsl_attrib_e
{
    GLSLATTRIB_POSITION = 0,
    GLSLATTRIB_COLOR = 1,
    GLSLATTRIB_TEXCOORD0 = 2,
    GLSLATTRIB_TEXCOORD1 = 3,
    GLSLATTRIB_TEXCOORD2 = 4,
    GLSLATTRIB_TEXCOORD3 = 5,
    GLSLATTRIB_TEXCOORD4 = 6,
    GLSLATTRIB_TEXCOORD5 = 7,
    GLSLATTRIB_TEXCOORD6 = 8,
    GLSLATTRIB_TEXCOORD7 = 9,
}
glsl_attrib;

// this enum selects which of the glslshadermodeinfo entries should be used
typedef enum shadermode_e
{
    SHADERMODE_GENERIC, ///< (particles/HUD/etc) vertex color, optionally multiplied by one texture
    SHADERMODE_POSTPROCESS, ///< postprocessing shader (r_glsl_postprocess)
    SHADERMODE_DEPTH_OR_SHADOW, ///< (depthfirst/shadows) vertex shader only
    SHADERMODE_FLATCOLOR, ///< (lightmap) modulate texture by uniform color (q1bsp, q3bsp)
    SHADERMODE_VERTEXCOLOR, ///< (lightmap) modulate texture by vertex colors (q3bsp)
    SHADERMODE_LIGHTMAP, ///< (lightmap) modulate texture by lightmap texture (q1bsp, q3bsp)
    SHADERMODE_FAKELIGHT, ///< (fakelight) modulate texture by "fake" lighting (no lightmaps, no nothing)
    SHADERMODE_LIGHTDIRECTIONMAP_MODELSPACE, ///< (lightmap) use directional pixel shading from texture containing modelspace light directions (q3bsp deluxemap)
    SHADERMODE_LIGHTDIRECTIONMAP_TANGENTSPACE, ///< (lightmap) use directional pixel shading from texture containing tangentspace light directions (q1bsp deluxemap)
    SHADERMODE_LIGHTDIRECTIONMAP_FORCED_LIGHTMAP, // forced deluxemapping for lightmapped surfaces
    SHADERMODE_LIGHTDIRECTIONMAP_FORCED_VERTEXCOLOR, // forced deluxemapping for vertexlit surfaces
    SHADERMODE_LIGHTDIRECTION, ///< (lightmap) use directional pixel shading from fixed light direction (q3bsp)
    SHADERMODE_LIGHTSOURCE, ///< (lightsource) use directional pixel shading from light source (rtlight)
    SHADERMODE_REFRACTION, ///< refract background (the material is rendered normally after this pass)
    SHADERMODE_WATER, ///< refract background and reflection (the material is rendered normally after this pass)
    SHADERMODE_DEFERREDGEOMETRY, ///< (deferred) render material properties to screenspace geometry buffers
    SHADERMODE_DEFERREDLIGHTSOURCE, ///< (deferred) use directional pixel shading from light source (rtlight) on screenspace geometry buffers
    SHADERMODE_COUNT
}
shadermode_t;

typedef enum shaderpermutation_e
{
    SHADERPERMUTATION_DIFFUSE = 1<<0, ///< (lightsource) whether to use directional shading
    SHADERPERMUTATION_VERTEXTEXTUREBLEND = 1<<1, ///< indicates this is a two-layer material blend based on vertex alpha (q3bsp)
    SHADERPERMUTATION_VIEWTINT = 1<<2, ///< view tint (postprocessing only), use vertex colors (generic only)
    SHADERPERMUTATION_COLORMAPPING = 1<<3, ///< indicates this is a colormapped skin
    SHADERPERMUTATION_SATURATION = 1<<4, ///< saturation (postprocessing only)
    SHADERPERMUTATION_FOGINSIDE = 1<<5, ///< tint the color by fog color or black if using additive blend mode
    SHADERPERMUTATION_FOGOUTSIDE = 1<<6, ///< tint the color by fog color or black if using additive blend mode
    SHADERPERMUTATION_FOGHEIGHTTEXTURE = 1<<7, ///< fog color and density determined by texture mapped on vertical axis
    SHADERPERMUTATION_FOGALPHAHACK = 1<<8, ///< fog color and density determined by texture mapped on vertical axis
    SHADERPERMUTATION_GAMMARAMPS = 1<<9, ///< gamma (postprocessing only)
    SHADERPERMUTATION_CUBEFILTER = 1<<10, ///< (lightsource) use cubemap light filter
    SHADERPERMUTATION_GLOW = 1<<11, ///< (lightmap) blend in an additive glow texture
    SHADERPERMUTATION_BLOOM = 1<<12, ///< bloom (postprocessing only)
    SHADERPERMUTATION_SPECULAR = 1<<13, ///< (lightsource or deluxemapping) render specular effects
    SHADERPERMUTATION_POSTPROCESSING = 1<<14, ///< user defined postprocessing (postprocessing only)
    SHADERPERMUTATION_REFLECTION = 1<<15, ///< normalmap-perturbed reflection of the scene infront of the surface, preformed as an overlay on the surface
    SHADERPERMUTATION_OFFSETMAPPING = 1<<16, ///< adjust texcoords to roughly simulate a displacement mapped surface
    SHADERPERMUTATION_OFFSETMAPPING_RELIEFMAPPING = 1<<17, ///< adjust texcoords to accurately simulate a displacement mapped surface (requires OFFSETMAPPING to also be set!)
    SHADERPERMUTATION_SHADOWMAP2D = 1<<18, ///< (lightsource) use shadowmap texture as light filter
    SHADERPERMUTATION_SHADOWMAPVSDCT = 1<<19, ///< (lightsource) use virtual shadow depth cube texture for shadowmap indexing
    SHADERPERMUTATION_SHADOWMAPORTHO = 1<<20, ///< (lightsource) use orthographic shadowmap projection
    SHADERPERMUTATION_DEFERREDLIGHTMAP = 1<<21, ///< (lightmap) read Texture_ScreenDiffuse/Specular textures and add them on top of lightmapping
    SHADERPERMUTATION_ALPHAKILL = 1<<22, ///< (deferredgeometry) discard pixel if diffuse texture alpha below 0.5, (generic) apply global alpha
    SHADERPERMUTATION_REFLECTCUBE = 1<<23, ///< fake reflections using global cubemap (not HDRI light probe)
    SHADERPERMUTATION_NORMALMAPSCROLLBLEND = 1<<24, ///< (water) counter-direction normalmaps scrolling
    SHADERPERMUTATION_BOUNCEGRID = 1<<25, ///< (lightmap) use Texture_BounceGrid as an additional source of ambient light
    SHADERPERMUTATION_BOUNCEGRIDDIRECTIONAL = 1<<26, ///< (lightmap) use 16-component pixels in bouncegrid texture for directional lighting rather than standard 4-component
    SHADERPERMUTATION_TRIPPY = 1<<27, ///< use trippy vertex shader effect
    SHADERPERMUTATION_DEPTHRGB = 1<<28, ///< read/write depth values in RGB color coded format for older hardware without depth samplers
    SHADERPERMUTATION_ALPHAGEN_VERTEX = 1<<29, ///< alphaGen vertex
    SHADERPERMUTATION_SKELETAL = 1<<30, ///< (skeletal models) use skeletal matrices to deform vertices (gpu-skinning)
    SHADERPERMUTATION_OCCLUDE = 1<<31, ///< use occlusion buffer for corona
    SHADERPERMUTATION_COUNT = 32 ///< size of shaderpermutationinfo array
}
shaderpermutation_t;

#endif
