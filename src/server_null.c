#include "quakedef.h"
#include "image.h"

#include "cl_dyntexture.h"
#include "keys.h"
// TODO: this file will be removed in future

dp_fonts_t dp_fonts;
keydest_t    key_dest;

// keys
int            key_consoleactive;
int            key_linepos;
char        key_line[MAX_INPUTLINE];
int chat_mode;
char        chat_buffer[MAX_INPUTLINE];
unsigned int    chat_bufferlen = 0;
qboolean    key_insert = true;    // insert key toggle (for editing)

void Key_Event (int key, int ascii, qboolean down) {
}
void Key_Init (void) {
}
void Key_Shutdown (void) {
}
const char *Key_GetBind (int key, int bindmap) {
    return NULL;
}

void Key_GetBindMap(int *fg, int *bg) {
}

qboolean Key_SetBindMap(int fg, int bg) {
    return false;
}

qboolean Key_SetBinding (int keynum, int bindmap, const char *binding) {
    return false;
}

int Key_StringToKeynum (const char *str) {
    return 0;
}

void Key_EventQueue_Block(void) {
}

void Key_EventQueue_Unblock(void) {
}

void Key_WriteBindings (qfile_t *f) {
}

// cl_input
cvar_t cl_upspeed = {CVAR_SAVE, "cl_upspeed","400","vertical movement speed (while swimming or flying)"};
cvar_t cl_yawspeed = {CVAR_SAVE, "cl_yawspeed","140","keyboard yaw turning speed"};
cvar_t cl_pitchspeed = {CVAR_SAVE, "cl_pitchspeed","150","keyboard pitch turning speed"};
cvar_t cl_sidespeed = {CVAR_SAVE, "cl_sidespeed","350","strafe movement speed"};
cvar_t cl_forwardspeed = {CVAR_SAVE, "cl_forwardspeed","400","forward movement speed"};
cvar_t cl_anglespeedkey = {CVAR_SAVE, "cl_anglespeedkey","1.5","how much +speed multiplies keyboard turning speed"};
cvar_t cl_movespeedkey = {CVAR_SAVE, "cl_movespeedkey","2.0","how much +speed multiplies keyboard movement speed"};
cvar_t cl_backspeed = {CVAR_SAVE, "cl_backspeed","400","backward movement speed"};

// cl_parse
cvar_t developer_networkentities = {0, "developer_networkentities", "0", "prints received entities, value is 0-10 (higher for more info, 10 being the most verbose)"};
cvar_t cl_nettimesyncboundmode = {CVAR_SAVE, "cl_nettimesyncboundmode", "6", "method of restricting client time to valid values, 0 = no correction, 1 = tight bounding (jerky with packet loss), 2 = loose bounding (corrects it if out of bounds), 3 = leniant bounding (ignores temporary errors due to varying framerate), 4 = slow adjustment method from Quake3, 5 = slighttly nicer version of Quake3 method, 6 = bounding + Quake3"};

void CL_ParseServerMessage(void) {
}

void CL_Parse_Init(void) {
}

void CL_Parse_ErrorCleanUp(void) {
}

void CL_Parse_Shutdown(void) {
}


/*
==================
CL_KeepaliveMessage

When the client is taking a long time to load stuff, send keepalive messages
so the server doesn't disconnect.
==================
*/

void CL_KeepaliveMessage (qboolean readmessages)
{
    static double lastdirtytime = 0;
    static qboolean recursive = false;
    double dirtytime;
    double deltatime;
    static double countdownmsg = 0;
    static double countdownupdate = 0;

    qboolean thisrecursive;

    thisrecursive = recursive;
    recursive = true;

    dirtytime = Sys_DirtyTime();
    deltatime = dirtytime - lastdirtytime;
    lastdirtytime = dirtytime;
    if (deltatime <= 0 || deltatime >= 1800.0)
        return;

    countdownmsg -= deltatime;
    countdownupdate -= deltatime;


    // no need if server is local and definitely not if this is a demo
    if (sv.active || !cls.netcon || cls.signon >= SIGNONS)
    {
        recursive = thisrecursive;
        return;
    }
}

void CL_VM_PreventInformationLeaks() {
}
void CL_NewFrameReceived(int num) {
}
void CL_MoveLerpEntityStates(entity_t *ent) {
}
void CL_VM_UpdateShowingScoresState(int showingscores) {
}

unsigned int palette_bgra_transparent[256];


void R_RegisterModule(const char *name, void(*start)(void), void(*shutdown)(void), void(*newmap)(void), void(*devicelost)(void), void(*devicerestored)(void)) {
}

void R_Modules_Shutdown(void) {
}

void R_Modules_Start(void) {
}

void R_Modules_Init(void) {
}

void R_Model_Sprite_Draw(entity_render_t *ent) {
}

void R_Q1BSP_Draw(entity_render_t *ent) {
}

void R_Q1BSP_DrawDepth(entity_render_t *ent) {
}

void R_Q1BSP_DrawDebug(entity_render_t *ent) {
}

void R_Q1BSP_DrawPrepass(entity_render_t *ent) {
}

void R_Q1BSP_CompileShadowMap(entity_render_t *ent, vec3_t relativelightorigin, vec3_t relativelightdirection, float lightradius, int numsurfaces, const int *surfacelist) {
}

void R_Q1BSP_DrawShadowMap(int side, entity_render_t *ent, const vec3_t relativelightorigin, const vec3_t relativelightdirection, float lightradius, int modelnumsurfaces, const int *modelsurfacelist, const unsigned char *surfacesides, const vec3_t lightmins, const vec3_t lightmaxs) {
}

void R_Q1BSP_DrawLight(entity_render_t *ent, int numsurfaces, const int *surfacelist, const unsigned char *lighttrispvs) {
}


cvar_t r_ambient = {0, "r_ambient", "0", "brightens map, value is 0-128"};


void R_Q1BSP_CompileShadowVolume(entity_render_t *ent, vec3_t relativelightorigin, vec3_t relativelightdirection, float lightradius, int numsurfaces, const int *surfacelist) {
}

void R_Q1BSP_DrawShadowVolume(entity_render_t *ent, const vec3_t relativelightorigin, const vec3_t relativelightdirection, float lightradius, int modelnumsurfaces, const int *modelsurfacelist, const vec3_t lightmins, const vec3_t lightmaxs) {
}

void R_Q1BSP_DrawSky(entity_render_t *ent) {
}

void R_Q1BSP_DrawAddWaterPlanes(entity_render_t *ent) {
}

void R_Q1BSP_GetLightInfo(entity_render_t *ent, vec3_t relativelightorigin, float lightradius, vec3_t outmins, vec3_t outmaxs, int *outleaflist, unsigned char *outleafpvs, int *outnumleafspointer, int *outsurfacelist, unsigned char *outsurfacepvs, int *outnumsurfacespointer, unsigned char *outshadowtrispvs, unsigned char *outlighttrispvs, unsigned char *visitingleafpvs, int numfrustumplanes, const mplane_t *frustumplanes) {
}

void CL_Particles_Shutdown (void) {
}

void CL_Particles_Init (void) {
}

void R_Particles_Init (void) {
}

void CL_ParticleTrail(int effectnameindex, float pcount, const vec3_t originmins, const vec3_t originmaxs, const vec3_t velocitymins, const vec3_t velocitymaxs, entity_t *ent, int palettecolor, qboolean spawndlight, qboolean spawnparticles, float tintmins[4], float tintmaxs[4], float fade) {
}

void CL_EntityParticles (const entity_t *ent) {
}

skinframe_t *decalskinframe;
cvar_t cl_decals_models = {CVAR_SAVE, "cl_decals_models", "0", "enables decals on animated models (if newsystem is also 1)"};
cvar_t cl_decals_bias = {CVAR_SAVE, "cl_decals_bias", "0.125", "distance to bias decals from surface to prevent depth fighting"};
cvar_t cl_decals_max = {CVAR_SAVE, "cl_decals_max", "4096", "maximum number of decals allowed to exist in the world at once"};
cvar_t cl_decals_time = {CVAR_SAVE, "cl_decals_time", "20", "how long before decals start to fade away"};
cvar_t cl_decals_newsystem = {CVAR_SAVE, "cl_decals_newsystem", "1", "enables new advanced decal system"};
cvar_t cl_decals_newsystem_intensitymultiplier = {CVAR_SAVE, "cl_decals_newsystem_intensitymultiplier", "2", "boosts intensity of decals (because the distance fade can make them hard to see otherwise)"};
cvar_t r_drawparticles = {0, "r_drawparticles", "1", "enables drawing of particles"};
cvar_t r_drawdecals = {0, "r_drawdecals", "1", "enables drawing of decals"};
cvar_t cl_decals_fadetime = {CVAR_SAVE, "cl_decals_fadetime", "1", "how long decals take to fade away"};

int R_PicmipForFlags(int flags) {
    return 0;
}

void R_TextureStats_Print(qboolean printeach, qboolean printpool, qboolean printtotal) {
}

void R_FreeTexturePool(rtexturepool_t **rtexturepool) {
}

rtexture_t *R_LoadTexture3D(rtexturepool_t *rtexturepool, const char *identifier, int width, int height, int depth, const unsigned char *data, textype_t textype, int flags, int miplevel, const unsigned int *palette) {
    return NULL;
}

void R_FreeTexture(rtexture_t *rt) {
}

rtexture_t *R_LoadTexture2D(rtexturepool_t *rtexturepool, const char *identifier, int width, int height, const unsigned char *data, textype_t textype, int flags, int miplevel, const unsigned int *palette) {
    return NULL;
}

rtexture_t *R_LoadTextureCubeMap(rtexturepool_t *rtexturepool, const char *identifier, int width, const unsigned char *data, textype_t textype, int flags, int miplevel, const unsigned int *palette) {
    return NULL;
}

rtexture_t *R_LoadTextureShadowMap2D(rtexturepool_t *rtexturepool, const char *identifier, int width, int height, textype_t textype, qboolean filter) {
    return NULL;
}
int R_SaveTextureDDSFile(rtexture_t *rt, const char *filename, qboolean skipuncompressed, qboolean hasalpha) {
    return -2;
}

rtexture_t *R_LoadTextureDDSFile(rtexturepool_t *rtexturepool, const char *filename, qboolean srgb, int flags, qboolean *hasalphaflag, float *avgcolor, int miplevel, qboolean optionaltexture) {
    return NULL;
}

void R_Textures_Init (void) {
}

int R_TextureFlags(rtexture_t *rt) {
    return 0;
}

void R_Textures_Frame (void) {
}

int R_RealGetTexture(rtexture_t *rt) {
    return 0;
}

cvar_t gl_max_lightmapsize = {CVAR_SAVE, "gl_max_lightmapsize", "1024", "maximum allowed texture size for lightmap textures, use larger values to improve rendering speed, as long as there is enough video memory available (setting it too high for the hardware will cause very bad performance)"};
cvar_t gl_texturecompression = {CVAR_SAVE, "gl_texturecompression", "0", "whether to compress textures, a value of 0 disables compression (even if the individual cvars are 1), 1 enables fast (low quality) compression at startup, 2 enables slow (high quality) compression at startup"};
cvar_t gl_texturecompression_sprites = {CVAR_SAVE, "gl_texturecompression_sprites", "1", "whether to compress sprites"};
cvar_t gl_texturecompression_normal = {CVAR_SAVE, "gl_texturecompression_normal", "0", "whether to compress normalmap (normalmap) textures"};
cvar_t gl_texturecompression_lightcubemaps = {CVAR_SAVE, "gl_texturecompression_lightcubemaps", "1", "whether to compress light cubemaps (spotlights and other light projection images)"};
cvar_t gl_texturecompression_q3bsplightmaps = {CVAR_SAVE, "gl_texturecompression_q3bsplightmaps", "0", "whether to compress lightmaps in q3bsp format levels"};
cvar_t gl_texturecompression_color = {CVAR_SAVE, "gl_texturecompression_color", "1", "whether to compress colormap (diffuse) textures"};
cvar_t gl_texturecompression_q3bspdeluxemaps = {CVAR_SAVE, "gl_texturecompression_q3bspdeluxemaps", "0", "whether to compress deluxemaps in q3bsp format levels (only levels compiled with q3map2 -deluxe have these)"};

rtexture_t * CL_GetDynTexture( const char *name ) {
    return NULL;
}

void R_MeshQueue_AddTransparent(dptransparentsortcategory_t category, const vec3_t center, void (*callback)(const entity_render_t *ent, const rtlight_t *rtlight, int numsurfaces, int *surfacelist), const entity_render_t *ent, int surfacenumber, const rtlight_t *rtlight) {
}

void R_MeshQueue_BeginScene(void) {
}

void R_MeshQueue_RenderTransparent(void) {
}

cvar_t vid_conwidth = {CVAR_SAVE, "vid_conwidth", "640", "virtual width of 2D graphics system"};
cvar_t vid_conheight = {CVAR_SAVE, "vid_conheight", "480", "virtual height of 2D graphics system"};
cvar_t scr_conbrightness = {CVAR_SAVE, "scr_conbrightness", "1", "brightness of console background (0 = black, 1 = image)"};
cvar_t r_letterbox = {0, "r_letterbox", "0", "reduces vertical height of view to simulate a letterboxed movie effect (can be used by mods for cutscenes)"};
cvar_t scr_conalpha = {CVAR_SAVE, "scr_conalpha", "1", "opacity of console background gfx/conback"};
cvar_t scr_conalphafactor = {CVAR_SAVE, "scr_conalphafactor", "1", "opacity of console background gfx/conback relative to scr_conalpha; when 0, gfx/conback is not drawn"};
cvar_t scr_conalpha2factor = {CVAR_SAVE, "scr_conalpha2factor", "0", "opacity of console background gfx/conback2 relative to scr_conalpha; when 0, gfx/conback2 is not drawn"};
cvar_t scr_conalpha3factor = {CVAR_SAVE, "scr_conalpha3factor", "0", "opacity of console background gfx/conback3 relative to scr_conalpha; when 0, gfx/conback3 is not drawn"};
cvar_t scr_conforcewhiledisconnected = {0, "scr_conforcewhiledisconnected", "1", "forces fullscreen console while disconnected"};
cvar_t scr_conscroll_x = {CVAR_SAVE, "scr_conscroll_x", "0", "scroll speed of gfx/conback in x direction"};
cvar_t scr_conscroll_y = {CVAR_SAVE, "scr_conscroll_y", "0", "scroll speed of gfx/conback in y direction"};
cvar_t scr_conscroll2_x = {CVAR_SAVE, "scr_conscroll2_x", "0", "scroll speed of gfx/conback2 in x direction"};
cvar_t scr_conscroll2_y = {CVAR_SAVE, "scr_conscroll2_y", "0", "scroll speed of gfx/conback2 in y direction"};
cvar_t scr_conscroll3_x = {CVAR_SAVE, "scr_conscroll3_x", "0", "scroll speed of gfx/conback3 in x direction"};
cvar_t scr_conscroll3_y = {CVAR_SAVE, "scr_conscroll3_y", "0", "scroll speed of gfx/conback3 in y direction"};
cvar_t r_stereo_separation = {0, "r_stereo_separation", "4", "separation distance of eyes in the world (negative values are only useful for cross-eyed viewing)"};
cvar_t r_stereo_angle = {0, "r_stereo_angle", "0", "separation angle of eyes (makes the views look different directions, as an example, 90 gives a 90 degree separation where the views are 45 degrees left and 45 degrees right)"};
int speedstringcount, r_timereport_active;
float scr_con_current;
float scr_centertime_off;
int r_stereo_side;
rtexture_t *loadingscreentexture = NULL;

void R_TimeReport(const char *desc) {
}

void SCR_CenterPrint(const char *str) {
}

void SCR_PopLoadingScreen (qboolean redraw) {
}

void SCR_PushLoadingScreen (qboolean redraw, const char *msg, float len_in_parent) {
}

void CL_UpdateScreen(void) {
}

void CL_Screen_Init(void) {
}

void CL_Screen_Shutdown(void) {
}

void CL_Screen_NewMap(void) {
}

void R_ClearScreen(qboolean fogcolor) {
}

qboolean R_Stereo_Active(void) {
    return false;
}

qboolean R_Stereo_ColorMasking(void) {
    return false;
}

void SCR_ClearLoadingScreen (qboolean redraw) {
}

void Sbar_Init (void) {
}

void V_Init (void) {
}

void V_DriftPitch (void) {
}
void V_FadeViewFlashs(void) {
}

void V_CalcViewBlend(void) {
}

float V_CalcRoll (const vec3_t angles, const vec3_t velocity) {
    return 0;
}

void V_CalcRefdef (void) {
}

cvar_t chase_active = {CVAR_SAVE, "chase_active", "0", "enables chase cam"};
// IMPORTANT
cvar_t cl_viewmodel_scale = {0, "cl_viewmodel_scale", "1", "changes size of gun model, lower values prevent poking into walls but cause strange artifacts on lighting and especially r_stereo/vid_stereobuffer options where the size of the gun becomes visible"};

trace_t CL_TraceLine(const vec3_t start, const vec3_t end, int type, prvm_edict_t *passedict, int hitsupercontentsmask, int skipsupercontentsmask, float extend, qboolean hitnetworkbrushmodels, qboolean hitnetworkplayers, int *hitnetworkentity, qboolean hitcsqcentities, qboolean hitsurfaces) {
    trace_t trace;
    memset (&trace, 0 , sizeof(trace_t));
    return trace;
}

dp_model_t *CL_GetModelByIndex(int modelindex) {
    return NULL;
}

void CL_FindNonSolidLocation(const vec3_t in, vec3_t out, vec_t radius) {
}

dp_model_t *CL_GetModelFromEdict(prvm_edict_t *ed) {
    return NULL;
}

// new remove: gl_backend, gl_rmain r_shadows, vid_shared
r_refdef_t r_refdef;
viddef_t vid;
qboolean vid_activewindow = true;
qboolean vid_hidden = true;
rtexture_t *r_texture_notexture;
rtexture_t *r_texture_blanknormalmap;
float in_mouse_x, in_mouse_y;

void VID_Start(void) {
}
void VID_Stop(void) {
}

skinframe_t *R_SkinFrame_LoadMissing(void) {
    return NULL;
}
void Render_Init(void) {
}
void R_Mesh_DestroyMeshBuffer(r_meshbuffer_t *buffer) {
}

r_meshbuffer_t *R_Mesh_CreateMeshBuffer(const void *data, size_t size, const char *name, qboolean isindexbuffer, qboolean isuniformbuffer, qboolean isdynamic, qboolean isindex16) {
    return NULL;
}

skinframe_t *R_SkinFrame_LoadExternal(const char *name, int textureflags, qboolean complain) {
    return NULL;
}

skinframe_t *R_SkinFrame_LoadInternalQuake(const char *name, int textureflags, int loadpantsandshirt, int loadglowtexture, const unsigned char *skindata, int width, int height) {
    return NULL;
}

rtexture_t *R_GetCubemap(const char *basename) {
    return NULL;
}

void R_SkinFrame_MarkUsed(skinframe_t *skinframe) {
}

int R_Shadow_GetRTLightInfo(unsigned int lightindex, float *origin, float *radius, float *color) {
    return 0;
}
skinframe_t *R_SkinFrame_LoadInternalBGRA(const char *name, int textureflags, const unsigned char *skindata, int width, int height, qboolean sRGB) {
    return NULL;
}
void FOG_clear(void) {
}

void R_DecalSystem_Reset(decalsystem_t *decalsystem) {
}

void R_RTLight_Update(rtlight_t *rtlight, int isstatic, matrix4x4_t *matrix, vec3_t color, int style, const char *cubemapname, int shadow, vec_t corona, vec_t coronasizescale, vec_t ambientscale, vec_t diffusescale, vec_t specularscale, int flags) {
}

void GL_Mesh_ListVBOs(qboolean printeach) {
}

cvar_t vid_sRGB = {CVAR_SAVE, "vid_sRGB", "0", "if hardware is capable, modify rendering to be gamma corrected for the sRGB color standard (computer monitors, TVs), recommended"};
cvar_t vid_sRGB_fallback = {CVAR_SAVE, "vid_sRGB_fallback", "0", "do an approximate sRGB fallback if not properly supported by hardware (2: also use the fallback if framebuffer is 8bit, 3: always use the fallback even if sRGB is supported)"};
cvar_t gl_paranoid = {0, "gl_paranoid", "0", "enables OpenGL error checking and other tests"};
cvar_t r_lerpsprites = {CVAR_SAVE, "r_lerpsprites", "0", "enables animation smoothing on sprites"};
cvar_t r_lerpmodels = {CVAR_SAVE, "r_lerpmodels", "1", "enables animation smoothing on models"};
cvar_t gl_printcheckerror = {0, "gl_printcheckerror", "0", "prints all OpenGL error checks, useful to identify location of driver crashes"};
cvar_t r_drawworld = {0, "r_drawworld","1", "draw world (most static stuff)"};
cvar_t r_speeds = {0, "r_speeds","0", "displays rendering statistics and per-subsystem timings"};
cvar_t r_fullbright = {0, "r_fullbright","0", "makes map very bright and renders faster"};
cvar_t r_equalize_entities_fullbright = {CVAR_SAVE, "r_equalize_entities_fullbright", "0", "render fullbright entities by equalizing their lightness, not by not rendering light"};
cvar_t r_dynamic = {CVAR_SAVE, "r_dynamic","1", "enables dynamic lights (rocket glow and such)"};
cvar_t r_lerplightstyles = {CVAR_SAVE, "r_lerplightstyles", "0", "enable animation smoothing on flickering lights"};
cvar_t r_smoothnormals_areaweighting = {0, "r_smoothnormals_areaweighting", "1", "uses significantly faster (and supposedly higher quality) area-weighted vertex normals and tangent vectors rather than summing normalized triangle normals and tangents"};
cvar_t r_fullbrights = {CVAR_SAVE, "r_fullbrights", "1", "enables glowing pixels in quake textures (changes need r_restart to take effect)"};
cvar_t r_shadow_lightattenuationdividebias = {0, "r_shadow_lightattenuationdividebias", "1", "changes attenuation texture generation"};
cvar_t r_shadow_lightattenuationlinearscale = {0, "r_shadow_lightattenuationlinearscale", "2", "changes attenuation texture generation"};

// cl_main.c
client_static_t    cls;
client_state_t    cl;
cvar_t csqc_progname = {0, "csqc_progname","csprogs.dat","name of csprogs.dat file to load"};
cvar_t csqc_progcrc = {CVAR_READONLY, "csqc_progcrc","-1","CRC of csprogs.dat file to load (-1 is none), only used during level changes and then reset to -1"};
cvar_t csqc_progsize = {CVAR_READONLY, "csqc_progsize","-1","file size of csprogs.dat file to load (-1 is none), only used during level changes and then reset to -1"};
cvar_t csqc_usedemoprogs = {0, "csqc_usedemoprogs","1","use csprogs stored in demos"};
cvar_t cl_locs_enable = {CVAR_SAVE, "locs_enable", "1", "enables replacement of certain % codes in chat messages: %l (location), %d (last death location), %h (health), %a (armor), %x (rockets), %c (cells), %r (rocket launcher status), %p (powerup status), %w (weapon status), %t (current time in level)"};
cvar_t cl_stainmaps_clearonload = {CVAR_SAVE, "cl_stainmaps_clearonload", "1","clear stainmaps on map restart"};

void CL_ExpandEntities(int num) {
}

void CL_Disconnect() {
}

void CL_EstablishConnection(const char *host, int firstarg) {
}

void CL_Init (void) {
}

void CL_SetInfo(const char *key, const char *value, qboolean send, qboolean allowstarkey, qboolean allowmodel, qboolean quiet)
{
}

void CL_Shutdown (void)
{
}

void CL_Locs_FindLocationName(char *buffer, size_t buffersize, vec3_t point)
{
}

void CL_UpdateWorld(void)
{
}

void CL_Disconnect_f(void)
{
    if (sv.active)
        Host_ShutdownServer();
}
