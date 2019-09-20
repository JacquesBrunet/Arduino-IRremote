#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//
//
//                              THOMSON
//
//
//==============================================================================

#define THOMSON_BITS          24  // The number of bits in the command

#define THOMSON_HDR_MARK    4000  // The length of the Header:Mark
#define THOMSON_HDR_SPACE   4000  // The lenght of the Header:Space

#define THOMSON_BIT_MARK    500  // The length of a Bit:Mark
#define THOMSON_ONE_SPACE   2000  // The length of a Bit:Space for 1's
#define THOMSON_ZERO_SPACE  1000  // The length of a Bit:Space for 0's

//#define THOMSON_BIT_STOP    3000  // Other things you may need to define

//+=============================================================================
//
#if SEND_THOMSON
void  IRsend::sendThomson (unsigned long data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(56);

	// Header
	mark (THOMSON_HDR_MARK);
	space(THOMSON_HDR_SPACE);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			mark (THOMSON_BIT_MARK);
			space(THOMSON_ONE_SPACE);
		} else {
			mark (THOMSON_BIT_MARK);
			space(THOMSON_ZERO_SPACE);
		}
	}

	// Footer
	mark(THOMSON_BIT_MARK);
    space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
//
#if DECODE_THOMSON
bool  IRrecv::decodeThomson (decode_results *results)
{
	unsigned long  data   = 0;  // Somewhere to build our code
	int            offset = 1;  // Skip the Gap reading

	// Check we have the right amount of data
	if (irparams.rawlen != 1 + 2 + (2 * THOMSON_BITS) + 1)  return false ;

	// Check initial Mark+Space match
	if (!MATCH_MARK (results->rawbuf[offset++], THOMSON_HDR_MARK ))  return false ;
	if (!MATCH_SPACE(results->rawbuf[offset++], THOMSON_HDR_SPACE))  return false ;

	// Read the bits in
	for (int i = 0;  i < THOMSON_BITS;  i++) {
		// Each bit looks like: MARK + SPACE_1 -> 1
		//                 or : MARK + SPACE_0 -> 0
		if (!MATCH_MARK(results->rawbuf[offset++], THOMSON_BIT_MARK))  return false ;

		// IR data is big-endian, so we shuffle it in from the right:
		if      (MATCH_SPACE(results->rawbuf[offset], THOMSON_ONE_SPACE))   data = (data << 1) | 1 ;
		else if (MATCH_SPACE(results->rawbuf[offset], THOMSON_ZERO_SPACE))  data = (data << 1) | 0 ;
		else                                                        return false ;
		offset++;
	}

	// Success
	results->bits        = THOMSON_BITS;
	results->value       = data;
	results->decode_type = THOMSON;
	return true;
}
#endif
