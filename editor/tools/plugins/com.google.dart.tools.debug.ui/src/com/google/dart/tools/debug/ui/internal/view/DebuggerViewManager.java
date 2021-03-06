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
package com.google.dart.tools.debug.ui.internal.view;

import com.google.dart.tools.debug.ui.internal.DartDebugUIPlugin;
import com.google.dart.tools.debug.ui.internal.DartUtil;
import com.google.dart.tools.debug.ui.internal.hover.DartDebugHover;
import com.google.dart.tools.ui.internal.text.editor.DartTextHover;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.core.DebugEvent;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.IDebugEventSetListener;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchListener;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.core.model.IDebugTarget;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.debug.ui.contexts.ISuspendTrigger;
import org.eclipse.debug.ui.contexts.ISuspendTriggerListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.TextConsole;

/**
 * Manages the Debugger view during the debug session.
 */
public class DebuggerViewManager implements ILaunchListener, ISuspendTriggerListener,
    IDebugEventSetListener {

  private static DebuggerViewManager manager;

  private static DartDebugHover hoverHelper = new DartDebugHover();

  public static void dispose() {
    if (manager != null) {
      DebugPlugin.getDefault().removeDebugEventListener(manager);
      DebugPlugin.getDefault().getLaunchManager().removeLaunchListener(manager);

      DartTextHover.removeContributer(hoverHelper);

      manager = null;
    }
  }

  public static DebuggerViewManager getDefault() {
    if (manager == null) {
      manager = new DebuggerViewManager();

      DebugPlugin.getDefault().getLaunchManager().addLaunchListener(
          DebuggerViewManager.getDefault());
      DebugPlugin.getDefault().addDebugEventListener(DebuggerViewManager.getDefault());
      DartTextHover.addContributer(hoverHelper);

    }

    return manager;
  }

  private DebuggerPatternMatchListener patternMatchListener = new DebuggerPatternMatchListener();

  DebuggerViewManager() {

  }

  @Override
  public void handleDebugEvents(DebugEvent[] events) {
    for (DebugEvent event : events) {
      if (event.getKind() == DebugEvent.CREATE && event.getSource() instanceof IProcess) {
        attachConsoleListener((IProcess) event.getSource());
      } else if (event.getKind() == DebugEvent.TERMINATE
          && event.getSource() instanceof IDebugTarget) {
        removeDebugTarget((IDebugTarget) event.getSource());
      }
    }
  }

  @Override
  public void launchAdded(ILaunch launch) {
    try {
      if (launch.getLaunchConfiguration().getType().getIdentifier().startsWith("com.google")
          && launch.getLaunchMode().equals(ILaunchManager.DEBUG_MODE)) {

        //  add the suspend trigger listener 
        ISuspendTrigger trigger = (ISuspendTrigger) launch.getAdapter(ISuspendTrigger.class);
        if (trigger != null) {
          trigger.addSuspendTriggerListener(this);
        }

      }
    } catch (CoreException e) {
      DartUtil.logError(e);
    }
  }

  @Override
  public void launchChanged(ILaunch launch) {

  }

  @Override
  public void launchRemoved(ILaunch launch) {
    try {
      if (launch.getLaunchConfiguration().getType().getIdentifier().startsWith("com.google") //$NON-NLS-N$
          && launch.getLaunchMode().equals(ILaunchManager.DEBUG_MODE)) {

        ISuspendTrigger trigger = (ISuspendTrigger) launch.getAdapter(ISuspendTrigger.class);

        if (trigger != null) {
          trigger.removeSuspendTriggerListener(this);
        }
      }
    } catch (CoreException e) {
      DartUtil.logError(e);
    }
  }

  @Override
  public void suspended(ILaunch launch, Object context) {
    Display.getDefault().asyncExec(new Runnable() {
      @Override
      public void run() {
        openDebuggerView();
        IWorkbenchWindow window = DartDebugUIPlugin.getDefault().getWorkbench().getActiveWorkbenchWindow();

        if (window == null) {
          window = getWindowWithView(DebuggerView.ID);
        }

        window.getShell().forceActive();
        IViewReference viewReference = window.getActivePage().findViewReference(DebuggerView.ID);
        window.getActivePage().activate(viewReference.getPart(true));
      }
    });
  }

  private void attachConsoleListener(IProcess process) {
    IConsole console = DebugUITools.getConsole(process);

    if (console instanceof TextConsole) {
      TextConsole textConsole = (TextConsole) console;

      textConsole.addPatternMatchListener(patternMatchListener);
    }
  }

  private IWorkbenchWindow getWindowWithView(String viewId) {
    IWorkbenchWindow[] windows = PlatformUI.getWorkbench().getWorkbenchWindows();

    if (windows.length > 0) {
      for (int i = 0; i < windows.length; i++) {
        IWorkbenchPage[] pages = windows[i].getPages();

        for (IWorkbenchPage page : pages) {
          if (page.findView(viewId) != null) {
            return windows[i];
          }
        }

      }
    }

    return null;
  }

  private void openDebuggerView() {
    try {
      IWorkbenchWindow window = DartDebugUIPlugin.getDefault().getWorkbench().getActiveWorkbenchWindow();

      if (window == null) {
        IWorkbenchWindow[] windows = PlatformUI.getWorkbench().getWorkbenchWindows();

        if (windows.length > 0) {
          IWorkbenchPage[] pages = windows[0].getPages();

          if (pages.length > 0) {
            pages[0].showView(DebuggerView.ID);
          }
        }
      } else {
        window.getActivePage().showView(DebuggerView.ID);
      }
    } catch (PartInitException e) {
      DartUtil.logError(e);
    }
  }

  private void removeDebugTarget(IDebugTarget target) {
    target.getLaunch().removeDebugTarget(target);
  }

}
