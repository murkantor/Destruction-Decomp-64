#ifndef M2C_H
#define M2C_H

/*
 * Types and field access used by C generated with m2c --valid-syntax
 * (tools/autodecomp.py). Replace them with real types when cleaning a file up.
 */

typedef s32 M2C_UNK;
typedef s8 M2C_UNK8;
typedef s16 M2C_UNK16;
typedef s32 M2C_UNK32;
typedef s64 M2C_UNK64;

/* A field of an unknown struct: the value of type *type_ptr at byte offset in expr. */
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))

#endif
