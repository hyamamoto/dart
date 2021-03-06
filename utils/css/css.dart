// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#library('css');

#import("../lib/file_system.dart");
#import('../lib/file_system_memory.dart');

#source('cssoptions.dart');
#source('source.dart');
#source('tokenkind.dart');
#source('token.dart');
#source('tokenizer_base.dart');
#source('tokenizer.dart');
#source('treebase.dart');
#source('tree.dart');
#source('cssselectorexception.dart');
#source('cssworld.dart');
#source('parser.dart');
#source('validate.dart');
#source('generate.dart');
#source('world.dart');


void initCssWorld([bool commandLine = true]) {
  FileSystem fs = new MemoryFileSystem();
  parseOptions([], fs);
  initializeWorld(fs);

  // TODO(terry): Should be set by arguments.  When run as a tool these aren't
  // set when run internaly set these so we can compile CSS and catch any
  // problems programmatically.
  options.throwOnErrors = true;
  options.throwOnFatal = true;
  options.useColors = commandLine ? true : false;
  options.warningsAsErrors = false;
  options.showWarnings = true;
}

// TODO(terry): Add obfuscation mapping file.
void cssParseAndValidate(String cssExpression, CssWorld cssworld) {
  Parser parser = new Parser(new SourceFile(SourceFile.IN_MEMORY_FILE,
      cssExpression));
  var tree = parser.parseTemplate();
  if (tree != null) {
    Validate.template(tree.selectors, cssworld);
  }
}

// Returns pretty printed tree of the expression.
String cssParseAndValidateDebug(String cssExpression, CssWorld cssworld) {
  Parser parser = new Parser(new SourceFile(SourceFile.IN_MEMORY_FILE,
      cssExpression));
  String output = "";
  String prettyTree = "";
  try {
    var tree = parser.parseTemplate();
    if (tree != null) {
      prettyTree = tree.toDebugString();
      Validate.template(tree.selectors, cssworld);
      output = prettyTree;
    }
  } catch (var e) {
    String error = e.toString();
    output = "$error\n$prettyTree";
    throw e;
  }

  return output;
}
