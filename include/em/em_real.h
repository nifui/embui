/*
 * Defines a float type.
 * To swap out the float type define EM_DEFINE_REAL and the subsequent implementations unless C
 * supports operators on the type. This must be done prior to including the header file.
 */

#pragma once

#ifndef EM_REAL_H
#define EM_REAL_H

#ifndef EM_REAL

#define EM_REAL float
#define EM_REAL_MUL(x, y) ((x) * (y))
#define EM_REAL_DIV(x, y) ((x) / (y))
#define EM_REAL_ADD(x, y) ((x) + (y))
#define EM_REAL_SUB(x, y) ((x) - (y))

#endif

#endif
