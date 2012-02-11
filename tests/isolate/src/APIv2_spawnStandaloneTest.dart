// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

// example of spawning an isolate from a closure
#library('spawn_tests');

child(ReceivePort port) {
  port.receive((msg, reply) => reply.send("re: $msg"));
}

main() {
  ReceivePort port = new ReceivePort();
  port.receive((msg, _) {
    Expect.equals("re: hi", msg);
    port.close();
  });

  Isolate2 c = new Isolate2.fromCode(child);
  c.sendPort.send("hi", port.toSendPort());
}