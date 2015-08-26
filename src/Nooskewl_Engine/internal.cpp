#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map_logic.h"
#include "Nooskewl_Engine/vertex_cache.h"

using namespace Nooskewl_Engine;

#ifdef NOOSKEWL_ENGINE_WINDOWS
HINSTANCE dll_handle;
#else
void *so_handle;
#endif

namespace Nooskewl_Engine {

Module m;

void load_dll()
{
#ifdef __APPLE__
	std::string filename = "libgame.dylib";
	so_handle = dlopen(filename.c_str(), RTLD_LAZY);

	if (so_handle == 0) {
		throw FileNotFoundError("Couldn't load shared library");
	}

	m.dll_start = (DLL_Start)dlsym(so_handle, "dll_start");
	m.dll_end = (DLL_End)dlsym(so_handle, "dll_end");
	m.dll_pause = (DLL_Pause)dlsym(so_handle, "dll_pause");
	m.dll_get_map_logic = (Map_Logic_Getter)dlsym(so_handle, "dll_get_map_logic");
	m.dll_get_brain = (Brain_Getter)dlsym(so_handle, "dll_get_brain");

	if (m.dll_start != 0 && m.dll_end != 0 && m.dll_pause != 0 && m.dll_get_map_logic != 0 && m.dll_get_brain != 0) {
		infomsg("Using %s\n", filename.c_str());
		return;
	}
#else
#if defined NOOSKEWL_ENGINE_WINDOWS
	List_Directory ld("*.dll");
#else
	List_Directory ld("*.so");
#endif
	std::string filename;

	while ((filename = ld.next()) != "") {
#ifdef NOOSKEWL_ENGINE_WINDOWS
		dll_handle = LoadLibrary(filename.c_str());

		if (!dll_handle) {
			throw FileNotFoundError("Couldn't load DLL");
		}

		m.dll_start = (DLL_Start)GetProcAddress(dll_handle, "dll_start");
		m.dll_end = (DLL_End)GetProcAddress(dll_handle, "dll_end");
		m.dll_pause = (DLL_Pause)GetProcAddress(dll_handle, "dll_pause");
		m.dll_get_map_logic = (Map_Logic_Getter)GetProcAddress(dll_handle, "dll_get_map_logic");
		m.dll_get_brain = (Brain_Getter)GetProcAddress(dll_handle, "dll_get_brain");

		if (m.dll_start != 0 && m.dll_end != 0 && m.dll_pause != 0 && m.dll_get_map_logic != 0 && m.dll_get_brain != 0) {
			infomsg("Using %s\n", filename.c_str());
			return;
		}
#else
		so_handle = dlopen(filename.c_str(), RTLD_LAZY);

		if (so_handle == 0) {
			throw FileNotFoundError("Couldn't load shared library");
		}

		m.dll_start = (DLL_Start)dlsym(so_handle, "dll_start");
		m.dll_end = (DLL_End)dlsym(so_handle, "dll_end");
		m.dll_pause = (DLL_Pause)dlsym(so_handle, "dll_pause");
		m.dll_get_map_logic = (Map_Logic_Getter)dlsym(so_handle, "dll_get_map_logic");
		m.dll_get_brain = (Brain_Getter)dlsym(so_handle, "dll_get_brain");

		if (m.dll_start != 0 && m.dll_end != 0 && m.dll_pause != 0 && m.dll_get_map_logic != 0 && m.dll_get_brain != 0) {
			infomsg("Using %s\n", filename.c_str());
			return;
		}
#endif
	}
#endif

	throw FileNotFoundError("Couldn't find a game DLL");
}

void close_dll()
{
#ifdef NOOSKEWL_ENGINE_WINDOWS
	FreeLibrary(dll_handle);
#else
	dlclose(so_handle);
#endif
}

#ifdef NOOSKEWL_ENGINE_WINDOWS

int c99_vsnprintf(char* str, int size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int c99_snprintf(char* str, int size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#endif // NOOSKEWL_ENGINE_WINDOWS

void errormsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	printf("ERROR: ");
	vprintf(fmt, v);
	va_end(v);
}

void infomsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
}

void printGLerror(const char *fmt, ...)
{
	GLenum error;
	if ((error = glGetError()) == GL_NO_ERROR) {
		return;
	}
	va_list v;
	va_start(v, fmt);
	printf("OPENGL ERROR (%d): ", error);
	vprintf(fmt, v);
	va_end(v);
	printf("\n");
}

int SDL_fgetc(SDL_RWops *file)
{
	unsigned char c;
	if (SDL_RWread(file, &c, 1, 1) == 0) {
		return EOF;
	}
	return c;
}

int SDL_fputc(int c, SDL_RWops *file)
{
	return SDL_RWwrite(file, &c, 1, 1) == 1 ? 1 : EOF;
}

char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max)
{
	size_t c = 0;
	while (c < max) {
		int i = SDL_fgetc(file);
		if (i == -1) {
			break;
		}
		buf[c] = (char)i;
		c++;
		if (i == '\n') {
			break;
		}
	}
	if (c == 0) return 0;
	buf[c] = 0;
	return buf;
}

int SDL_fputs(const char *string, SDL_RWops *file)
{
	unsigned int len = strlen(string);
	return SDL_RWwrite(file, string, 1, len) < len ? EOF : 0;
}

void SDL_fprintf(SDL_RWops *file, const char *fmt, ...)
{
	char buf[1000];
	va_list v;
	va_start(v, fmt);
	vsnprintf(buf, 1000, fmt, v);
	va_end(v);

	SDL_fputs(buf, file);
}

SDL_RWops *open_file(std::string filename)
{
	SDL_RWops *file = noo.cpa->open(filename);
	if (file == 0) {
		throw FileNotFoundError(filename);
	}
	return file;
}

std::string itos(int i)
{
	char buf[20];
	snprintf(buf, 20, "%d", i);
	return std::string(buf);
}

int check_args(int argc, char **argv, std::string arg)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], arg.c_str())) {
			return i;
		}
	}
	return -1;
}

std::string string_printf(const char *fmt, ...)
{
	char buf[1000];
	va_list v;
	va_start(v, fmt);
	vsnprintf(buf, 1000, fmt, v);
	va_end(v);

	return buf;
}

std::string escape_string(std::string s, char c)
{
	std::string ret;

	for (int i = 0; i < (int)s.length(); i++) {
		if (s[i] == c) {
			ret += "\\";
		}
		ret += s.substr(i, 1);
	}

	return ret;
}

std::string unescape_string(std::string s)
{
	std::string ret;

	for (int i = 0; i < (int)s.length(); i++) {
		if (s[i] != '\\') {
			ret += s.substr(i, 1);
		}
	}

	return ret;
}

std::string unescape_string(std::string);

#ifdef NOOSKEWL_ENGINE_WINDOWS
List_Directory::List_Directory(std::string filespec) :
	got_first(false),
	done(false)
{
	handle = FindFirstFile(filespec.c_str(), &ffd);
	if (handle == 0) {
		done = true;
	}
}

List_Directory::~List_Directory()
{
	FindClose(handle);
}

std::string List_Directory::next()
{
	if (done) {
		return "";
	}

	if (got_first == true) {
		if (FindNextFile(handle, &ffd) == 0) {
			done = true;
		}
	}
	else {
		got_first = true;
	}

	return ffd.cFileName;
}
#else
List_Directory::List_Directory(std::string filespec) :
	i(0)
{
	gl.gl_pathv = 0;

	int ret = glob(filespec.c_str(), 0, 0, &gl);

	if (ret != 0) {
		i = 0;
	}
}

List_Directory::~List_Directory()
{
	free(gl.gl_pathv);
}

std::string List_Directory::next()
{
	if (i >= gl.gl_pathc) {
		i = -1;
	}

	if (i < 0) {
		return "";
	}

	return gl.gl_pathv[i++];
}
#endif // NOOSKEWL_ENGINE_WINDOWS

#ifdef NOOSKEWL_ENGINE_WINDOWS
/* The following Windows icon and other mouse cursor creation code comes from Allegro, http://liballeg.org */

#define WINDOWS_RGB(r,g,b)  ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

static BITMAPINFO *get_bitmap_info(Size<int> size)
{
	BITMAPINFO *bi;
	int i;

	bi = (BITMAPINFO *)new Uint8[sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256];

	ZeroMemory(&bi->bmiHeader, sizeof(BITMAPINFOHEADER));

	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biBitCount = 32;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biWidth = size.w;
	bi->bmiHeader.biHeight = -size.h;
	bi->bmiHeader.biClrUsed = 256;
	bi->bmiHeader.biCompression = BI_RGB;

	for (i = 0; i < 256; i++) {
		bi->bmiColors[i].rgbRed = 0;
		bi->bmiColors[i].rgbGreen = 0;
		bi->bmiColors[i].rgbBlue = 0;
		bi->bmiColors[i].rgbReserved = 0;
	}

	return bi;
}

static void stretch_blit_to_hdc(BYTE *pixels, Size<int> size, HDC dc, int src_x, int src_y, int src_w, int src_h, int dest_x, int dest_y, int dest_w, int dest_h)
{
	const int bitmap_h = size.h;
	const int bottom_up_src_y = bitmap_h - src_y - src_h;
	BITMAPINFO *bi;

	bi = get_bitmap_info(size);

	if (bottom_up_src_y == 0 && src_x == 0 && src_h != bitmap_h) {
		StretchDIBits(dc, dest_x, dest_h+dest_y-1, dest_w, -dest_h, src_x, bitmap_h - src_y + 1, src_w, -src_h, pixels, bi, DIB_RGB_COLORS, SRCCOPY);
	}
	else {
		StretchDIBits(dc, dest_x, dest_y, dest_w, dest_h, src_x, bottom_up_src_y, src_w, src_h, pixels, bi, DIB_RGB_COLORS, SRCCOPY);
	}

	delete[] bi;
}

HICON win_create_icon(HWND wnd, Uint8 *data, Size<int> size, int xfocus, int yfocus, bool is_cursor)
{
	int x, y;
	int sys_sm_cx, sys_sm_cy;
	HDC h_dc;
	HDC h_and_dc;
	HDC h_xor_dc;
	ICONINFO iconinfo;
	HBITMAP and_mask;
	HBITMAP xor_mask;
	HBITMAP hOldAndMaskBitmap;
	HBITMAP hOldXorMaskBitmap;
	HICON icon;

	Uint8 *tmp = new Uint8[size.area() * 4];
	for (y = 0; y < size.h; y++) {
		Uint8 *src = data + y * (size.w * 4);
		Uint8 *dst = tmp + (size.h-y-1) * (size.w * 4); // flip y
		for (x = 0; x < size.w; x++) {
			Uint8 r = *src++;
			Uint8 g = *src++;
			Uint8 b = *src++;
			Uint8 a = *src++;
			*dst++ = b;
			*dst++ = g;
			*dst++ = r;
			*dst++ = a;
		}
	}

	/*
	if (is_cursor) {
		sys_sm_cx = GetSystemMetrics(SM_CXCURSOR);
		sys_sm_cy = GetSystemMetrics(SM_CYCURSOR);
	}
	else {
		sys_sm_cx = GetSystemMetrics(SM_CXICON);
		sys_sm_cy = GetSystemMetrics(SM_CYICON);
	}
	*/

	sys_sm_cx = size.w;
	sys_sm_cy = size.h;

	/* Create bitmap */
	h_dc = GetDC(wnd);
	h_xor_dc = CreateCompatibleDC(h_dc);
	h_and_dc = CreateCompatibleDC(h_dc);

	/* Prepare AND (monochrome) and XOR (colour) mask */
	and_mask = CreateBitmap(sys_sm_cx, sys_sm_cy, 1, 1, 0);
	xor_mask = CreateCompatibleBitmap(h_dc, sys_sm_cx, sys_sm_cy);
	hOldAndMaskBitmap = (HBITMAP) SelectObject(h_and_dc, and_mask);
	hOldXorMaskBitmap = (HBITMAP) SelectObject(h_xor_dc, xor_mask);

	/* Create transparent cursor */
	for (y = 0; y < sys_sm_cy; y++) {
		for (x = 0; x < sys_sm_cx; x++) {
			SetPixel(h_and_dc, x, y, WINDOWS_RGB(255, 255, 255));
			SetPixel(h_xor_dc, x, y, WINDOWS_RGB(0, 0, 0));
		}
	}

	stretch_blit_to_hdc((BYTE *)tmp, size, h_xor_dc, 0, 0, size.w, size.h, 0, 0, size.w, size.h);

	/* Make cursor background transparent */
	for (y = 0; y < size.h; y++) {
		Uint8 *p = tmp + y * (size.w * 4);
		for (x = 0; x < size.w; x++) {

			Uint8 b = *p++;
			Uint8 g = *p++;
			Uint8 r = *p++;
			Uint8 a = *p++;

			if (a != 0) {
				/* Don't touch XOR value */
				SetPixel(h_and_dc, x, y, 0);
			}
			else {
				/* No need to touch AND value */
				SetPixel(h_xor_dc, x, y, WINDOWS_RGB(0, 0, 0));
			}
		}
	}

	SelectObject(h_and_dc, hOldAndMaskBitmap);
	SelectObject(h_xor_dc, hOldXorMaskBitmap);
	DeleteDC(h_and_dc);
	DeleteDC(h_xor_dc);
	ReleaseDC(wnd, h_dc);

	iconinfo.fIcon = is_cursor ? false : true;
	iconinfo.xHotspot = xfocus;
	iconinfo.yHotspot = yfocus;
	iconinfo.hbmMask = and_mask;
	iconinfo.hbmColor = xor_mask;

	icon = CreateIconIndirect(&iconinfo);

	DeleteObject(and_mask);
	DeleteObject(xor_mask);

	delete[] tmp;

	return icon;
}
#endif

#ifdef __linux__
X11::Cursor x_create_cursor(X11::Display *display, Uint8 *data, Size<int> size, int xfocus, int yfocus)
{
	X11::XcursorImage *image = X11::XcursorImageCreate(size.w, size.h);

	for (int y = 0; y < size.h; y++) {
		Uint8 *src = data + (size.w * (size.h-1) * 4) - (size.w * y * 4);
		Uint8 *dst = ((Uint8 *)image->pixels) + size.w * y * 4;
		memcpy(dst, src, size.w * 4);
	}

	image->xhot = xfocus;
	image->yhot = yfocus;

	X11::Cursor cursor = X11::XcursorImageLoadCursor(display, image);

	X11::XcursorImageDestroy(image);

	return cursor;
}
#endif

} // End namespace Nooskewl_Engine
