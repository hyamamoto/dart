// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

interface DocumentFragment extends Element default _DocumentFragmentFactoryProvider {

  DocumentFragment();

  DocumentFragment.html(String html);

  // TODO(nweiz): enable these when XML and/or SVG are ported
  // /** WARNING: Currently this doesn't work on Dartium (issue 649). */
  // DocumentFragment.xml(String xml);
  // DocumentFragment.svg(String svg);

  DocumentFragment clone(bool deep);


  ElementEvents get on();

  Element query(String selectors);

}
