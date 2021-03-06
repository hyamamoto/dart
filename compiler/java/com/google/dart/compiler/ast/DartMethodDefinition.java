// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

package com.google.dart.compiler.ast;

import com.google.dart.compiler.resolver.Element;
import com.google.dart.compiler.resolver.MethodNodeElement;

import java.util.Collections;
import java.util.List;

/**
 * Represents a Dart method definition.
 */
public class DartMethodDefinition extends DartClassMember<DartExpression> {

  protected DartFunction function;
  private MethodNodeElement element;

  public static DartMethodDefinition create(DartExpression name,
                                            DartFunction function,
                                            Modifiers modifiers,
                                            List<DartInitializer> initializers) {
    if (initializers == null) {
      return new DartMethodDefinition(name, function, modifiers);
    } else {
      return new DartMethodWithInitializersDefinition(name, function, modifiers,
                                                      initializers);
    }
  }

  private DartMethodDefinition(DartExpression name, DartFunction function, Modifiers modifiers) {
    super(name, modifiers);
    this.function = becomeParentOf(function);
  }

  public DartFunction getFunction() {
    return function;
  }

  @Override
  public MethodNodeElement getElement() {
    return element;
  }

  @Override
  public void setElement(Element element) {
    this.element = (MethodNodeElement) element;
  }

  public List<DartInitializer> getInitializers() {
    return Collections.emptyList();
  }

  @Override
  public void visitChildren(ASTVisitor<?> visitor) {
    super.visitChildren(visitor);
    function.accept(visitor);
  }

  @Override
  public <R> R accept(ASTVisitor<R> visitor) {
    return visitor.visitMethodDefinition(this);
  }

  private static class DartMethodWithInitializersDefinition extends DartMethodDefinition {

    private final NodeList<DartInitializer> initializers = NodeList.create(this);

    DartMethodWithInitializersDefinition(DartExpression name,
                                         DartFunction function,
                                         Modifiers modifiers,
                                         List<DartInitializer> initializers) {
      super(name, function, modifiers);
      this.initializers.addAll(initializers);
    }

    @Override
    public List<DartInitializer> getInitializers() {
      return initializers;
    }

    @Override
    public void visitChildren(ASTVisitor<?> visitor) {
      super.visitChildren(visitor);
      initializers.accept(visitor);
    }
  }
}
