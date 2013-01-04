#pragma once

#define _WIN32_WINNT 0x0501 // for get/freeaddrinfo()

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmpex.h>
#include <malloc.h>
#include <memory.h>
#include <io.h>
#include <conio.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include <algorithm>
#include <array>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>
#include <digi_audio.h>

#pragma hdrstop
