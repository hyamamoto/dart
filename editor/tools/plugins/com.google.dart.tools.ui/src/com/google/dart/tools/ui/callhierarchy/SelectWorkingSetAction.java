/*
 * Copyright (c) 2012, the Dart project authors.
 * 
 * Licensed under the Eclipse Public License v1.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 * 
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */
package com.google.dart.tools.ui.callhierarchy;

import com.google.dart.tools.ui.internal.text.DartHelpContextIds;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.PlatformUI;

// TODO remove
class SelectWorkingSetAction extends Action {
  private final SearchScopeActionGroup fGroup;

  public SelectWorkingSetAction(SearchScopeActionGroup group) {
    super(CallHierarchyMessages.SearchScopeActionGroup_workingset_select_text);
    this.fGroup = group;
    setToolTipText(CallHierarchyMessages.SearchScopeActionGroup_workingset_select_tooltip);
    PlatformUI.getWorkbench().getHelpSystem().setHelp(this,
        DartHelpContextIds.CALL_HIERARCHY_SEARCH_SCOPE_ACTION);
  }

  @Override
  public void run() {
//    fGroup.setActiveWorkingSets(null);
  }
}
