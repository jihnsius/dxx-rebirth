#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_PYTHON
void cxx_script_get_guided_missile_rotang(vms_angvec *);
void cxx_script_get_player_ship_rotthrust(dxxobject *);
unsigned cxx_script_get_player_ship_vecthrust(dxxobject *, fix);
void cxx_script_hook_controls();
#else
static inline void cxx_script_get_guided_missile_rotang(vms_angvec *) {}
static inline void cxx_script_get_player_ship_rotthrust(vms_vector *) {}
static inline unsigned cxx_script_get_player_ship_vecthrust(dxxobject *, fix) { return 0; }
static inline void cxx_script_hook_controls() {}
#endif

#ifdef __cplusplus
}
#endif
