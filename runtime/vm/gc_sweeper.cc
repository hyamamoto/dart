// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/gc_sweeper.h"

#include "vm/freelist.h"
#include "vm/globals.h"
#include "vm/pages.h"

namespace dart {

intptr_t GCSweeper::SweepPage(HeapPage* page, FreeList* freelist) {
  // Keep track of the discovered live object sizes to be able to finish
  // sweeping early. Reset the per page in_use count for the next marking phase.
  intptr_t in_use_swept = 0;
  intptr_t in_use = page->used();
  page->set_used(0);

  uword current = page->first_object_start();
  uword top = page->top();

  while (current < top) {
    if (in_use_swept == in_use) {
      // No more marked objects will be found on this page.
      page->set_top(current);
      break;
    }
    RawObject* raw_obj = RawObject::FromAddr(current);
    intptr_t obj_size;
    if (raw_obj->IsMarked()) {
      // Found marked object. Clear the mark bit and update swept bytes.
      raw_obj->ClearMarkBit();
      obj_size = raw_obj->Size();
      in_use_swept += obj_size;
    } else {
      uword free_end = current + raw_obj->Size();
      while (free_end < top) {
        RawObject* next_obj = RawObject::FromAddr(free_end);
        if (next_obj->IsMarked()) {
          // Reached the end of the free block.
          break;
        }
        // Expand the free block by the size of this object.
        free_end += next_obj->Size();
      }
      obj_size = free_end - current;
      if ((current + obj_size) == top) {
        page->set_top(current);
        break;
      } else {
        freelist->Free(current, obj_size);
      }
    }
    current += obj_size;
  }

  return in_use_swept;
}


intptr_t GCSweeper::SweepLargePage(HeapPage* page) {
  RawObject* raw_obj = RawObject::FromAddr(page->first_object_start());
  if (!raw_obj->IsMarked()) {
    // The large object was not marked. Used size is zero, which also tells the
    // calling code that the large object page can be recycled.
    return 0;
  }
  raw_obj->ClearMarkBit();
  return raw_obj->Size();
}

}  // namespace dart
