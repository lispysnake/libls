/*
 * This file is part of libls.
 *
 * Copyright (c) 2017-2018 Ikey Doherty
 * Copyright (c) 2019 Lispy Snake, Ltd.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

/**
 * Define ls_unlikely(x) macro
 */
#ifndef ls_unlikely
#define ls_unlikely(x) __builtin_expect((x), 0)
#endif

/**
 * Define ls_likely(x) macro
 */
#ifndef ls_likely
#define ls_likely(x) __builtin_expect((x), 1)
#endif

/**
 * Helper to define some part of the code as unused, but placeholdered
 */
#ifndef __ls_unused__
#define __ls_unused__ __attribute__((unused))
#endif

/**
 * Helper to compute static array size
 */
#define LS_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifndef LS_PTR_TO_INT
#define LS_PTR_TO_INT(x) ((unsigned int)((uintptr_t)(x)))
#endif

#ifndef LS_INT_TO_PTR
#define LS_INT_TO_PTR(x) ((void *)((uintptr_t)x))
#endif

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
