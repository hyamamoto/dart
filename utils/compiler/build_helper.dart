// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#import('dart:io');
#import('dart:uri');

main() {
  List<String> arguments = new Options().arguments;
  Uri uri = new Uri(scheme: 'file', path: '${arguments[0]}/');
  String dartVmLocation = uri.resolve(arguments[1]).path;
  Uri productionLauncher = uri.resolve(arguments[2]);
  Uri developerLauncher = uri.resolve(arguments[3]);
  String launcherScript = buildScript(uri);

  writeScript(productionLauncher,
              ['#!$dartVmLocation\n',
               launcherScript]);
  writeScript(developerLauncher,
              ['#!$dartVmLocation --enable_checked_mode\n',
               launcherScript]);
}

writeScript(Uri uri, List<String> chunks) {
  var f = new File(uri.path);
  var stream = f.openSync(FileMode.WRITE);
  for (String chunk in chunks) {
    stream.writeStringSync(chunk);
  }
  stream.closeSync();

  // TODO(ahe): Make script executable.
  // TODO(ahe): Also make a .bat file for Windows.
}

buildScript(Uri uri) {
  return """

#import('dart:io');

#import('${uri.resolve('../../lib/compiler/implementation/dart2js.dart').path}');

class Helper {
  void run() {
    try {
      List<String> argv = ['--library-root=${uri.resolve('../../').path}'];
      argv.addAll(new Options().arguments);
      compile(argv);
    } catch (var exception, var trace) {
      try {
        print('Internal error: \$exception');
      } catch (var ignored) {
        print('Internal error: error while printing exception');
      }
      try {
        print(trace);
      } finally {
        exit(253); // 253 is recognized as a crash by our test scripts.
      }
    }
  }
}

void main() {
  new Helper().run();
}
""";
}
