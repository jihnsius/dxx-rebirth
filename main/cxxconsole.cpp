#include <cctype>
#include <vector>
#include "key.h"
#include "gamefont.h"
#include "gr.h"

extern "C"
{
	void cxx_handle_misc_con_key(const unsigned key);
	void cxx_con_interactive_print(int *const py);
}

enum
{
	e_max_input_size = 96,
};

typedef std::vector<char> input_container_t;

static input_container_t g_con_input_buffer;
static unsigned g_con_input_position;
static unsigned g_con_mode;

static const char con_mode_names[][4] =
{
	"",
};

static void con_input_printable_key(const char key)
{
	if (!key)
		return;
	const size_t size = g_con_input_buffer.size();
	if (size >= e_max_input_size)
		return;
	const unsigned con_input_position = g_con_input_position;
	g_con_input_buffer.insert(con_input_position >= size ? g_con_input_buffer.end() : g_con_input_buffer.begin() + con_input_position, &key, &key + 1);
	++ g_con_input_position;
}

static void con_input_backspace()
{
	const unsigned con_input_position = g_con_input_position;
	if (!con_input_position)
		return;
	const size_t size = g_con_input_buffer.size();
	if (con_input_position >= size)
		g_con_input_buffer.pop_back();
	else
		g_con_input_buffer.erase(g_con_input_buffer.begin() + con_input_position - 1);
	-- g_con_input_position;
}

static void con_input_tab()
{
}

static void con_input_enter()
{
}

static void con_input_left()
{
	if (!g_con_input_position)
		return;
	-- g_con_input_position;
}

static void con_input_right()
{
	const size_t size = g_con_input_buffer.size();
	if (g_con_input_position < size)
		++ g_con_input_position;
	else
		g_con_input_position = size;
}

static void con_input_delete()
{
	unsigned con_input_position = g_con_input_position;
	const size_t size = g_con_input_buffer.size();
	if (con_input_position >= size)
		return;
	g_con_input_buffer.erase(g_con_input_buffer.begin() + con_input_position);
}

static void con_input_ctrl_key(const char key)
{
	switch(key)
	{
		case 'k':
			g_con_input_buffer.erase(g_con_input_buffer.begin() + g_con_input_position, g_con_input_buffer.end());
			break;
		case 'u':
			{
				const auto begin = g_con_input_buffer.begin();
				g_con_input_buffer.erase(begin, begin + g_con_input_position);
				g_con_input_position = 0;
			}
			break;
		default:
			break;
	}
}

static unsigned map_shift_key(const unsigned ascii_value)
{
	switch(ascii_value)
	{
		case '1':
			return '!';
		case '2':
			return '@';
		case '3':
			return '#';
		case '4':
			return '$';
		case '5':
			return '%';
		case '6':
			return '^';
		case '7':
			return '&';
		case '8':
			return '*';
		case '9':
			return '(';
		case '0':
			return ')';
		case '-':
			return '_';
		case '=':
			return '+';
		case '[':
			return '{';
		case ']':
			return '}';
		case ';':
			return ':';
		case '\'':
			return '"';
		case '`':
			return '~';
		case '\\':
			return '|';
		case ',':
			return '<';
		case '.':
			return '>';
		case '/':
			return '?';
		case '*':
			return '*';
		case ' ':
			return ' ';
		case 'q':
		case 'w':
		case 'e':
		case 'r':
		case 't':
		case 'y':
		case 'u':
		case 'i':
		case 'o':
		case 'p':
		case 'a':
		case 's':
		case 'd':
		case 'f':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'l':
		case 'z':
		case 'x':
		case 'c':
		case 'v':
		case 'b':
		case 'n':
		case 'm':
			return toupper(ascii_value);
		default:
			return 0;
	}
}

void cxx_handle_misc_con_key(const unsigned key)
{
	const unsigned ascii_value = key_properties[key & 0xff].ascii_value;
	if (ascii_value != 255)
	{
		if (key < 0x100)
			con_input_printable_key(ascii_value);
		else if ((key & (KEY_SHIFTED | KEY_CTRLED | KEY_ALTED)) == KEY_SHIFTED)
			con_input_printable_key(map_shift_key(ascii_value));
		else if ((key & (KEY_SHIFTED | KEY_CTRLED | KEY_ALTED)) == KEY_CTRLED)
			con_input_ctrl_key(ascii_value);
		return;
	}
	if (ascii_value == 255)
	{
		const SDLKey k = key_properties[key & 0xff].sym;
		switch(k)
		{
			case SDLK_ESCAPE:
				break;
			case SDLK_BACKSPACE:
				con_input_backspace();
				break;
			case SDLK_TAB:
				con_input_tab();
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				con_input_enter();
				break;
			case SDLK_F1:
			case SDLK_F2:
			case SDLK_F3:
			case SDLK_F4:
			case SDLK_F5:
			case SDLK_F6:
			case SDLK_F7:
			case SDLK_F8:
			case SDLK_F9:
			case SDLK_F10:
			case SDLK_F11:
			case SDLK_F12:
				break;
			case SDLK_KP7:
			case SDLK_KP8:
			case SDLK_KP9:
			case SDLK_KP4:
			case SDLK_KP5:
			case SDLK_KP6:
			case SDLK_KP1:
			case SDLK_KP2:
			case SDLK_KP3:
			case SDLK_KP0:
				con_input_printable_key(k - SDLK_KP0 + '0');
				break;
			case SDLK_KP_MINUS:
				con_input_printable_key('-');
				break;
			case SDLK_KP_PLUS:
				con_input_printable_key('+');
				break;
			case SDLK_KP_PERIOD:
				con_input_printable_key('.');
				break;
			case SDLK_KP_DIVIDE:
				con_input_printable_key('/');
				break;
			case SDLK_PAUSE:
			case SDLK_PRINT:
			case SDLK_HOME:
			case SDLK_UP:
			case SDLK_PAGEUP:
				break;
			case SDLK_LEFT:
				con_input_left();
				break;
			case SDLK_RIGHT:
				con_input_right();
				break;
			case SDLK_END:
			case SDLK_DOWN:
			case SDLK_PAGEDOWN:
			case SDLK_INSERT:
				break;
			case SDLK_DELETE:
				con_input_delete();
				break;
			default:
			case SDLK_UNKNOWN:
				break;
		}
	}
}

void cxx_con_interactive_print(int *const py)
{
	if (g_con_input_buffer.empty())
		return;
	int& y = *py;
	gr_set_fontcolor(BM_XRGB(0, 0, 48), -1);
	int w,h,aw;
	const char *const line = &g_con_input_buffer[0];
	gr_get_string_size(line, &w, &h, &aw);
	y-=h+FSPACY(1);
	const size_t size = g_con_input_buffer.size();
	const unsigned con_input_position = g_con_input_position;
	gr_printf(FSPACX(1), y, "%s> %.*s" CC_UNDERLINE_S " %.*s", con_mode_names[g_con_mode], con_input_position, line, static_cast<int>(size - con_input_position), line + con_input_position);
}
