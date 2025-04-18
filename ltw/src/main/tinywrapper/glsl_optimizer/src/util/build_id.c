/*
 * Copyright Â© 2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifdef HAVE_DL_ITERATE_PHDR
#include <dlfcn.h>
#include <link.h>
#include <stddef.h>
#include <string.h>

#include "build_id.h"
#include "macros.h"

#ifndef NT_GNU_BUILD_ID
#define NT_GNU_BUILD_ID 3
#endif

#ifndef ElfW
#define ElfW(type) Elf_##type
#endif

struct build_id_note {
   ElfW(Nhdr) nhdr;

   char name[4]; /* Note name for build-id is "GNU\0" */
   uint8_t build_id[0];
};

struct callback_data {
   /* Base address of shared object, taken from Dl_info::dli_fbase */
   const void *dli_fbase;

   struct build_id_note *note;
};

static int
build_id_find_nhdr_callback(struct dl_phdr_info *info, size_t size, void *data_)
{
   struct callback_data *data = data_;

   /* Calculate address where shared object is mapped into the process space.
    * (Using the base address and the virtual address of the first LOAD segment)
    */
   void *map_start = NULL;
   for (unsigned i = 0; i < info->dlpi_phnum; i++) {
      if (info->dlpi_phdr[i].p_type == PT_LOAD) {
         map_start = (void *)(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
         break;
      }
   }

   if (map_start != data->dli_fbase)
      return 0;

   for (unsigned i = 0; i < info->dlpi_phnum; i++) {
      if (info->dlpi_phdr[i].p_type != PT_NOTE)
         continue;

      struct build_id_note *note = (void *)(info->dlpi_addr +
                                            info->dlpi_phdr[i].p_vaddr);
      ptrdiff_t len = info->dlpi_phdr[i].p_filesz;

      while (len >= sizeof(struct build_id_note)) {
         if (note->nhdr.n_type == NT_GNU_BUILD_ID &&
            note->nhdr.n_descsz != 0 &&
            note->nhdr.n_namesz == 4 &&
            memcmp(note->name, "GNU", 4) == 0) {
            data->note = note;
            return 1;
         }

         size_t offset = sizeof(ElfW(Nhdr)) +
                         ALIGN_POT(note->nhdr.n_namesz, 4) +
                         ALIGN_POT(note->nhdr.n_descsz, 4);
         note = (struct build_id_note *)((char *)note + offset);
         len -= offset;
      }
   }

   return 0;
}

const struct build_id_note *
build_id_find_nhdr_for_addr(const void *addr)
{
   Dl_info info;

   if (!dladdr(addr, &info))
      return NULL;

   if (!info.dli_fbase)
      return NULL;

   struct callback_data data = {
      .dli_fbase = info.dli_fbase,
      .note = NULL,
   };

   if (!dl_iterate_phdr(build_id_find_nhdr_callback, &data))
      return NULL;

   return data.note;
}

unsigned
build_id_length(const struct build_id_note *note)
{
   return note->nhdr.n_descsz;
}

const uint8_t *
build_id_data(const struct build_id_note *note)
{
   return note->build_id;
}

#endif
