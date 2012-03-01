// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

Map<String> foo;  // Static type warning: Map takes 2 type arguments.

main() {
  foo = null;  /// 00: dynamic type error
  var bar = new Map<String>();  /// 01: compile-time error
}
