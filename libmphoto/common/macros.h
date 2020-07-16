// Copyright 2020 Google LLC
// Author: pinheirojamie

#ifndef LIBMPHOTO_COMMON_MACROS_H_
#define LIBMPHOTO_COMMON_MACROS_H_

#include "absl/status/status.h"

#define RETURN_IF_ERROR(expr)              \
  {                                        \
    do {                                   \
      absl::Status status__ = expr;        \
      if (!status__.ok()) return status__; \
    } while (false);                       \
  }

#endif  // LIBMPHOTO_COMMON_MACROS_H_
