/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPLES_MACROS_H_
#define SAMPLES_MACROS_H_

#include <iostream>

#include "absl/status/status.h"

#define TERMINATE_IF_ERROR(expr)                  \
  {                                               \
    do {                                          \
      absl::Status status__ = expr;               \
      if (!status__.ok()) {                       \
        std::cout << status__ << std::endl;       \
        return static_cast<int>(status__.code()); \
      }                                           \
    } while (false);                              \
  }

#endif  // SAMPLES_MACROS_H_
