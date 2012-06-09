#include <algorithm>
#include <cctype>
#include <deque>
#include <vector>

#include <boost/iterator/reverse_iterator.hpp>
#include "key.h"
#include "gamefont.h"

using std::swap;

extern "C"
{
	void cxx_con_init();
	void cxx_handle_misc_con_key(const unsigned key);
	void cxx_con_interactive_print(int *const py);
}

enum
{
	e_max_input_size = 96,
};

typedef std::vector<char> input_container_t;
typedef std::deque<input_container_t> old_input_container_t;

static old_input_container_t g_con_history;
static input_container_t g_con_input_buffer;
static input_container_t g_con_kill_buffer;
static unsigned g_con_input_position;
/*
 * Position in the history list.  If 0, then not browsing history.
 * Otherwise, it is the one-based index into the history list, so
 * g_con_history_pos=1 means show the most recent history item.
 */
static unsigned g_con_history_pos;
static bool g_con_history_browse;

#ifdef USE_PYTHON
void scripting_init();
void scripting_input_enter(const char *);
#else
static void scripting_init()
{
}

static void scripting_input_enter(const char *)
{
}
#endif

static bool history_index_out_of_range(const unsigned history_pos)
{
	if (!history_pos)
		return true;
	if (history_pos > g_con_history.size())
		return true;
	return false;
}

void cxx_con_init()
{
	scripting_init();
}

template <typename T>
static void con_input_insert_range(const T begin, const T end)
{
	const size_t size = g_con_input_buffer.size();
	if (size >= e_max_input_size)
		return;
	size_t len = std::distance(begin, end);
	if (len > e_max_input_size - size)
		len = e_max_input_size - size;
	const unsigned con_input_position = g_con_input_position;
	g_con_input_buffer.insert(con_input_position >= size ? g_con_input_buffer.end() : g_con_input_buffer.begin() + con_input_position, begin, end);
	g_con_input_position += len;
}

static void con_input_printable_key(const char key)
{
	if (!key)
		return;
	con_input_insert_range(&key, &key + 1);
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

static bool blank_vector(const input_container_t& input_buffer)
{
	input_container_t::const_iterator ci = input_buffer.begin();
	if (ci == input_buffer.end())
		return true;
	while(isspace(static_cast<unsigned>(*ci)))
	{
		++ci;
		if (ci == input_buffer.end())
			return true;
	}
	if (!*ci)
		return true;
	return false;
}

static void con_history_add(input_container_t& input_buffer)
{
	old_input_container_t& history = g_con_history;
	if (history.front() == input_buffer)
		/*
		 * Do not save two successive copies of the same value.
		 * However, do save two non-adjacent copies of the same value.
		 */
		return;
	history.push_front(std::move(input_buffer));
	if (history.size() > 100)
		history.pop_back();
}

static void con_input_enter()
{
	input_container_t& input_buffer = g_con_input_buffer;
	input_buffer.push_back(0);
	const char *const input = &input_buffer[0];
	if (!blank_vector(input_buffer))
	{
		scripting_input_enter(input);
		con_history_add(input_buffer);
	}
	/*
	 * Although the g++-4.5.3 implementation of std::move seems to make
	 * the vector empty, the documentation only guarantees that the old
	 * vector be "valid", but not defined.  Therefore, clear it here to
	 * be really sure.
	 *
	 * This also allows con_history_add to return without modifying the
	 * vector, and still erase the input line on completion.
	 */
	input_buffer.clear();
	g_con_input_position = 0;
	g_con_history_pos = 0;
	g_con_history_browse = false;
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

static void con_input_kill_range(input_container_t& dst, input_container_t& src, const std::pair<input_container_t::iterator, input_container_t::iterator>& range)
{
	dst.assign(range.first, range.second);
	src.erase(range.first, range.second);
}

static void con_input_seek_home_key()
{
	g_con_input_position = 0;
}

static void con_input_seek_end_key()
{
	g_con_input_position = g_con_input_buffer.size();
}

static void con_input_seek_up_key()
{
	if (history_index_out_of_range(g_con_history_pos + 1))
		return;
	g_con_history_browse = true;
	++ g_con_history_pos;
}

static void con_input_seek_down_key()
{
	unsigned history_pos = g_con_history_pos;
	const unsigned size_history = g_con_history.size();
	if (history_pos > size_history)
		/*
		 * This should not happen, but trap it anyway, just in case.
		 */
		history_pos = size_history;
	if (!history_pos)
		return;
	-- history_pos;
	g_con_history_pos = history_pos;
	g_con_history_browse = !!history_pos;
}

static void con_input_kill_word()
{
	unsigned con_input_position = g_con_input_position;
	if (!con_input_position)
		return;
	const unsigned size = g_con_input_buffer.size();
	if (!size)
		return;
	const input_container_t::iterator ib = g_con_input_buffer.begin();
	const input_container_t::iterator ie = ib + std::min(con_input_position, size);
	input_container_t::iterator ii = ie - 1;
	const auto ap_alnum = [](const unsigned c) -> int { return isalnum(c) || isblank(c); };
	const auto ap_noalnum = [](const unsigned c) -> int { return !isalnum(c); };
	const auto a = isalnum(static_cast<unsigned>(*ii))
		? ap_alnum
		: ap_noalnum;
	typedef boost::reverse_iterator<input_container_t::iterator> reverse_iterator;
	ii = std::find_if(reverse_iterator(ii), reverse_iterator(ib), a).base();
	con_input_kill_range(g_con_kill_buffer, g_con_input_buffer, {ib, ii});
	g_con_input_position -= std::distance(ib, ii);
}

static void con_input_ctrl_key(const char key)
{
	switch(key)
	{
		case 'a':
			con_input_seek_home_key();
			break;
		case 'e':
			con_input_seek_end_key();
			break;
		case 'k':
			con_input_kill_range(g_con_kill_buffer, g_con_input_buffer, {g_con_input_buffer.begin() + g_con_input_position, g_con_input_buffer.end()});
			break;
		case 't':
			{
				const size_t size = g_con_input_buffer.size();
				if (size <= 1)
					break;
				unsigned con_input_position = g_con_input_position;
				if (con_input_position >= size)
					con_input_position = size - 1;
				if (!con_input_position)
					break;
				const input_container_t::iterator ib = g_con_input_buffer.begin() + con_input_position;
				const input_container_t::iterator ia = ib - 1;
				std::iter_swap(ia, ib);
			}
			break;
		case 'u':
			{
				const auto begin = g_con_input_buffer.begin();
				con_input_kill_range(g_con_kill_buffer, g_con_input_buffer, {begin, begin + g_con_input_position});
				g_con_input_position = 0;
			}
			break;
		case 'w':
			con_input_kill_word();
			break;
		case 'y':
			con_input_insert_range(g_con_kill_buffer.begin(), g_con_kill_buffer.end());
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

static bool con_is_history_navigation_key(const SDLKey k)
{
	if (k == SDLK_UP)
		return true;
	if (k == SDLK_DOWN)
		return true;
	return false;
}

static void history_sync()
{
	if (!g_con_history_browse)
		return;
	const unsigned history_pos = g_con_history_pos;
	if (history_index_out_of_range(history_pos))
		return;
	old_input_container_t& history = g_con_history;
	old_input_container_t::const_iterator ci = history.begin();
	std::advance(ci, history_pos - 1);
	old_input_container_t::const_reference r = *ci;
	g_con_input_buffer = r;
	// Remove the trailing null byte.
	g_con_input_buffer.pop_back();
	const unsigned input_size = static_cast<unsigned>(g_con_input_buffer.size());
	if (g_con_input_position)
		g_con_input_position = std::min(g_con_input_position, input_size);
	else
		g_con_input_position = input_size;
	g_con_history_browse = false;
}

void cxx_handle_misc_con_key(const unsigned key)
{
	const unsigned ascii_value = key_properties[key & 0xff].ascii_value;
	if (ascii_value != 255)
	{
		history_sync();
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
		if (!con_is_history_navigation_key(k))
			history_sync();
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
			case SDLK_PAGEUP:
				break;
			case SDLK_UP:
				con_input_seek_up_key();
				break;
			case SDLK_HOME:
				con_input_seek_home_key();
				break;
			case SDLK_END:
				con_input_seek_end_key();
				break;
			case SDLK_LEFT:
				con_input_left();
				break;
			case SDLK_RIGHT:
				con_input_right();
				break;
			case SDLK_DOWN:
				con_input_seek_down_key();
				break;
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
	const unsigned history_pos = g_con_history_pos;
	char buffer[512];
	if (g_con_history_browse)
	{
		if (history_index_out_of_range(history_pos))
			snprintf(buffer, sizeof(buffer), "%u!%u", history_pos, __LINE__);
		else
		{
			old_input_container_t& history = g_con_history;
			old_input_container_t::const_iterator ci = history.begin();
			std::advance(ci, history_pos - 1);
			old_input_container_t::const_reference r = *ci;
			snprintf(buffer, sizeof(buffer), "%u> %s", history_pos, &r[0]);
		}
	}
	else
	{
		if (g_con_input_buffer.empty())
			return;
		const char *const line = &g_con_input_buffer[0];
		const size_t size = g_con_input_buffer.size();
		const size_t con_input_position = g_con_input_position;
		if (con_input_position > size)
			snprintf(buffer, sizeof(buffer), "%u:%lu:%lu!%u", history_pos, con_input_position, size, __LINE__);
		else
			snprintf(buffer, sizeof(buffer), "%u> %.*s" CC_UNDERLINE_S " %.*s", history_pos, static_cast<unsigned>(con_input_position), line, static_cast<unsigned>(size - con_input_position), line + con_input_position);
	}
	const unsigned x = FSPACX(1);
	int w,h,aw;
	int& y = *py;
	gr_set_fontcolor(BM_XRGB(0, 0, 48), -1);
	gr_get_string_size(buffer, &w, &h, &aw);
	y-=h+FSPACY(1);
	gr_string(x, y, buffer);
}
