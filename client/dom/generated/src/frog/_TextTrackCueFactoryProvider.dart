// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

class _TextTrackCueFactoryProvider {
  factory TextTrackCue(String id, num startTime, num endTime, String text,
                       [String settings, bool pauseOnExit]) native
'''
if (settings == null)
  return new TextTrackCue(id, startTime, endTime, text);
if (pauseOnExit == null)
  return new TextTrackCue(id, startTime, endTime, text, settings);
return new TextTrackCue(id, startTime, endTime, text, settings, pauseOnExit);
''';
}
