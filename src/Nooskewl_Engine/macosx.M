#include "Nooskewl_Engine/basic_types.h"
#include "Nooskewl_Engine/macosx.h"

using namespace Nooskewl_Engine;

#import <Cocoa/Cocoa.h>

static NSCursor *cursor;

namespace Nooskewl_Engine {

void macosx_create_custom_cursor(Uint8 *data, Size<int> size, int x_focus, int y_focus)
{
	NSImage *img = [[NSImage alloc] initWithSize: NSMakeSize((float)size.w, (float)size.h)];

	NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes: NULL // Allocate memory yourself
		pixelsWide:size.w
		pixelsHigh:size.h
		bitsPerSample: 8
		samplesPerPixel: 4
		hasAlpha:YES
		isPlanar:NO
		colorSpaceName:NSDeviceRGBColorSpace
		bytesPerRow: 0 // Calculate yourself
		bitsPerPixel:0 ];// Calculate yourself

	int x, y;

	for (y = 0; y < size.h; y++) {
		unsigned char *src = data + (size.h-y-1) * (size.w * 4);
		unsigned char *dst = [rep bitmapData] + y * [rep bytesPerRow];
		for (x = 0; x < size.w; x++) {
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
		}
	}

	[img addRepresentation:rep];

	[rep release];

	cursor = [[NSCursor alloc] initWithImage: img hotSpot: NSMakePoint(x_focus, y_focus)];

	[img release];
}

void macosx_set_custom_cursor()
{
	[cursor set];
}

void macosx_destroy_custom_cursor()
{
	[cursor release];
}

} // End namespace Nooskewl_Engine
