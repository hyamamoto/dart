/*
 * Copyright 2012 Google Inc.
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
package com.google.dart.tools.ui.internal.intro;

import com.google.common.collect.Lists;
import com.google.dart.tools.core.DartCore;
import com.google.dart.tools.ui.DartToolsPlugin;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Platform;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Collections;
import java.util.List;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

/**
 * Helper for providing {@link SampleDescription}s.
 */
public final class SampleDescriptionHelper {
  /**
   * @return all {@link SampleDescription} from the "samples" directory.
   */
  public static List<SampleDescription> getDescriptions() throws Exception {
    List<SampleDescription> descriptions = Lists.newArrayList();
    File samplesDirectory = getSamplesDirectory();
    scanFolder(descriptions, samplesDirectory);
    Collections.sort(descriptions);
    return descriptions;
  }

  /**
   * Attempts to find sample description in the given directory, otherwise scans recursively.
   */
  static void scanFolder(List<SampleDescription> descriptions, File directory) throws Exception {
    // ignore not directories
    if (!directory.exists() || !directory.isDirectory()) {
      return;
    }

    // attempt to add description
    boolean added = addDescription(descriptions, directory);

    if (added) {
      return;
    }

    // scan children
    for (File child : directory.listFiles()) {
      scanFolder(descriptions, child);
    }
  }

  /**
   * Attempts to add {@link SampleDescription} for given directory.
   * 
   * @return <code>true</code> if {@link SampleDescription} was added.
   */
  private static boolean addDescription(final List<SampleDescription> descriptions,
      final File directory) {
    String sampleName = directory.getName();

    // attempt to find description
    final boolean descriptionAdded[] = {false};

    if (doesSampleResourceExist(sampleName + ".xml")) {
      try {
        final File logoFile = createSampleImageFile(sampleName + ".png");

        SAXParserFactory factory = SAXParserFactory.newInstance();
        SAXParser saxParser = factory.newSAXParser();
        DefaultHandler handler = new DefaultHandler() {
          private StringBuilder sb = new StringBuilder();
          private String name;
          private String filePath;
          private String descriptionText;

          @Override
          public void characters(char[] ch, int start, int length) throws SAXException {
            sb.append(ch, start, length);
          }

          @Override
          public void endElement(String uri, String localName, String qName) throws SAXException {
            if (qName.equals("description")) {
              descriptionText = sb.toString();
            }
            if (qName.equals("sample") && filePath != null && name != null
                && descriptionText != null) {
              descriptions.add(new SampleDescription(directory, filePath, name, descriptionText,
                  logoFile));
              descriptionAdded[0] = true;
            }
          }

          @Override
          public void startElement(String uri, String localName, String qName, Attributes attributes)
              throws SAXException {
            sb.setLength(0);
            if (qName.equals("sample")) {
              name = attributes.getValue("name");
              filePath = attributes.getValue("file");
            }
          }
        };

        InputStream is = getSampleResourceUrl(sampleName + ".xml").openStream();

        try {
          saxParser.parse(is, handler);
        } finally {
          is.close();
        }
      } catch (Throwable e) {
        DartCore.logError(e);
      }
    }

    // may be added
    return descriptionAdded[0];
  }

  private static File createSampleImageFile(String resourceName) throws IOException {
    try {
      URL fileUrl = FileLocator.toFileURL(getSampleResourceUrl(resourceName));

      return new File(fileUrl.toURI());
    } catch (URISyntaxException exception) {
      throw new IOException(exception);
    }
  }

  private static boolean doesSampleResourceExist(String resourceName) {
    return getSampleResourceUrl(resourceName) != null;
  }

  private static URL getSampleResourceUrl(String resourceName) {
    return DartToolsPlugin.getDefault().getBundle().getEntry("/samples/" + resourceName);
  }

  /**
   * @return the {@link File} of the "samples" directory.
   */
  private static File getSamplesDirectory() throws Exception {
    File installDir = new File(Platform.getInstallLocation().getURL().getFile());

    return new File(installDir, "samples");
  }

}
