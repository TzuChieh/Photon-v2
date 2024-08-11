#pragma once

/*! @file

@brief Useful macro definitions for general operations.
*/

#define PH_CONCAT_2(a, b) a##b
#define PH_CONCAT_3(a, b, c) a##b##c
#define PH_CONCAT_4(a, b, c, d) a##b##c##d
#define PH_CONCAT_5(a, b, c, d, e) a##b##c##d##e
#define PH_CONCAT_6(a, b, c, d, e, f) a##b##c##d##e##f
#define PH_CONCAT_7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define PH_CONCAT_8(a, b, c, d, e, f, g, h) a##b##c##d##e##f##g##h

/*! @brief Places an expression that does nothing.
Useful in situations where the macro expansion is intended to mimic a function scope expression
while doing nothing.
*/
#define PH_NO_OP() ((void)0)
