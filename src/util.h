/*
 * This file is part of libuf.
 *
 * Copyright © 2017-2018 Ikey Doherty
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#pragma once

/**
 * Define uf_unlikely(x) macro
 */
#ifndef uf_unlikely
#define uf_unlikely(x) __builtin_expect((x), 0)
#endif

/**
 * Define uf_likely(x) macro
 */
#ifndef uf_likely
#define uf_likely(x) __builtin_expect((x), 1)
#endif

/**
 * Helper to define some part of the code as unused, but placeholdered
 */
#ifndef __uf_unused__
#define __uf_unused__ __attribute__((unused))
#endif

#ifndef UF_PTR_TO_INT
#define UF_PTR_TO_INT(x) ((unsigned int)((uintptr_t)(x)))
#endif

#ifndef UF_INT_TO_PTR
#define UF_INT_TO_PTR(x) ((void *)((uintptr_t)x))
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
