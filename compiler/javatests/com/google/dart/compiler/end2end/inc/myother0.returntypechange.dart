// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

class Other0 {
  static int value() { return 42; }

  int field_;

  Other0() : this.field_ = 42 { }
  num get field() { return field_; }
}

int globalVar = 42;

int globalFunction() {
  return 42;
}
