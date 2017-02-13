/*
 * Copyright 2015 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef FOLLY_USE_JEMALLOC
#include "jemalloc/jemalloc.h"
#endif
#include <folly/Malloc.h>

#include <cstdint>

namespace folly {

// How do we determine that we're using jemalloc?
// In the hackiest way possible. We allocate memory using malloc() and see if
// the per-thread counter of allocated memory increases. This makes me feel
// dirty inside. Also note that this requires jemalloc to have been compiled
// with --enable-stats.
bool usingJEMallocSlow() {
  // Some platforms (*cough* OSX *cough*) require weak symbol checks to be
  // in the form if (mallctl != nullptr). Not if (mallctl) or if (!mallctl)
  // (!!). http://goo.gl/xpmctm

#ifdef FOLLY_USE_JEMALLOC
  mallocx = je_mallocx;
  rallocx = je_rallocx;
  xallocx = je_xallocx;
  sallocx = je_sallocx;
  dallocx = je_dallocx;
  nallocx = je_nallocx;
  mallctl = je_mallctl;

  if (mallocx == nullptr || rallocx == nullptr || xallocx == nullptr
      || sallocx == nullptr || dallocx == nullptr || nallocx == nullptr
      || mallctl == nullptr) {
      goto failed;
  }

  // "volatile" because gcc optimizes out the reads from *counter, because
  // it "knows" malloc doesn't modify global state...
  /* nolint */ volatile uint64_t* counter;
  size_t counterLen = sizeof(uint64_t*);

  if (mallctl("thread.allocatedp", static_cast<void*>(&counter), &counterLen,
              nullptr, 0) != 0) {
    goto failed;
  }

  if (counterLen != sizeof(uint64_t*)) {
    goto failed;
  }

  uint64_t origAllocated = *counter;

  void* ptr = je_malloc(1);
  if (!ptr) {
    // wtf, failing to allocate 1 byte
    printf("~_~ jemalloc failed. 1 \n");
    goto failed;
  }
  je_free(ptr);

  if (origAllocated == *counter)
     printf("~_~ jemalloc failed. 2 \n");
    goto failed;

  folly_malloc = je_malloc;
  folly_calloc = je_calloc;
  folly_realloc = je_realloc;
  folly_aligned_alloc = je_aligned_alloc;
  folly_free = je_free;
  folly_aligned_free = je_free;
  return true;

failed:
#endif
  folly_malloc = malloc;
  folly_calloc = calloc;
  folly_realloc = realloc;
  folly_aligned_alloc = _aligned_malloc;
  folly_free = free;
  folly_aligned_free = _aligned_free;
  return false;
}

}  // namespaces
