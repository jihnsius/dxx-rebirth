#pragma once

#include "vecmat.h"

#ifdef __cplusplus
struct dxxobject;

#ifdef USE_PYTHON
void cxx_script_get_guided_missile_rotang(vms_angvec *);
void cxx_script_get_player_ship_rotthrust(dxxobject *);
unsigned cxx_script_get_player_ship_vecthrust(dxxobject *, fix);
void cxx_script_hook_controls();
void py_load_level_hit();
void scripting_init();
void scripting_close();
void scripting_input_enter(const char *);
#else
static inline void cxx_script_get_guided_missile_rotang(vms_angvec *) {}
static inline void cxx_script_get_player_ship_rotthrust(vms_vector *) {}
static inline unsigned cxx_script_get_player_ship_vecthrust(dxxobject *, fix) { return 0; }
static inline void cxx_script_hook_controls() {}
static inline void py_load_level_hit() {}
static inline void scripting_init() {}
static inline void scripting_close() {}
static inline void scripting_input_enter(const char *) {}
#endif

#endif
