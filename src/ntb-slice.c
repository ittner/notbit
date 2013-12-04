/*
 * Notbit - A Bitmessage client
 * Copyright (C) 2013  Neil Roberts
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "ntb-slice.h"

void
ntb_slice_allocator_init(struct ntb_slice_allocator *allocator,
                         size_t size,
                         size_t alignment)
{
        allocator->element_size = MAX(size, sizeof (struct ntb_slice));
        allocator->element_alignment = alignment;
        allocator->magazine = NULL;
        ntb_slab_init(&allocator->slab);
}

void
ntb_slice_allocator_destroy(struct ntb_slice_allocator *allocator)
{
        ntb_slab_destroy(&allocator->slab);
}

void *
ntb_slice_alloc(struct ntb_slice_allocator *allocator)
{
        void *ret;

        if (allocator->magazine) {
                ret = allocator->magazine;
                allocator->magazine = allocator->magazine->next;
        } else {
                ret = ntb_slab_allocate(&allocator->slab,
                                        allocator->element_size,
                                        allocator->element_alignment);
        }

        return ret;
}

void
ntb_slice_free(struct ntb_slice_allocator *allocator,
               void *ptr)
{
        struct ntb_slice *slice = ptr;

        slice->next = allocator->magazine;
        allocator->magazine = slice;
}
