#pragma once

#ifdef __cplusplus

void cxx_con_init();
void cxx_con_close();
void cxx_con_add_buffer_line(const char *buffer);
void cxx_handle_misc_con_key(const unsigned key);
void cxx_con_handle_idle();
void cxx_con_interactive_print(int *const py);

#endif
