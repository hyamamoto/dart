// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

package com.google.dart.compiler.resolver;

import com.google.dart.compiler.ast.DartField;
import com.google.dart.compiler.ast.DartIdentifier;
import com.google.dart.compiler.ast.DartMethodDefinition;
import com.google.dart.compiler.ast.DartNode;
import com.google.dart.compiler.ast.Modifiers;
import com.google.dart.compiler.common.SourceInfo;
import com.google.dart.compiler.type.Type;

class FieldElementImplementation extends AbstractNodeElement implements FieldElement, FieldNodeElement {
  private final EnclosingElement holder;
  private final SourceInfo nameLocation;
  private Modifiers modifiers;
  private Type type;
  private MethodNodeElement getter;
  private MethodNodeElement setter;
  private Type constantType;

  FieldElementImplementation(DartNode node,
      SourceInfo nameLocation,
      String name,
      EnclosingElement holder,
      Modifiers modifiers) {
    super(node, name);
    this.holder = holder;
    this.modifiers = modifiers;
    this.nameLocation = nameLocation;
  }

  @Override
  public Type getType() {
    return type;
  }

  @Override
  public void setType(Type type) {
    this.type = type;
  }

  @Override
  public ElementKind getKind() {
    return ElementKind.FIELD;
  }

  @Override
  public SourceInfo getNameLocation() {
    return nameLocation;
  }

  @Override
  public EnclosingElement getEnclosingElement() {
    return holder;
  }

  @Override
  public Modifiers getModifiers() {
    return modifiers;
  }

  @Override
  public boolean isStatic() {
    return modifiers.isStatic();
  }

  public static FieldElementImplementation fromNode(DartField node,
      EnclosingElement holder,
      Modifiers modifiers) {
    return new FieldElementImplementation(node,
        node.getName().getSourceInfo(),
        node.getName().getName(),
        holder,
        modifiers);
  }

  public static FieldElementImplementation fromNode(DartMethodDefinition node,
      EnclosingElement holder,
      Modifiers modifiers) {
    return new FieldElementImplementation(node,
        node.getName().getSourceInfo(),
        ((DartIdentifier) node.getName()).getName(),
        holder,
        modifiers);
  }

  @Override
  public MethodNodeElement getGetter() {
    return getter;
  }

  @Override
  public MethodNodeElement getSetter() {
    return setter;
  }

  void setGetter(MethodNodeElement getter) {
    this.getter = getter;
  }

  void setSetter(MethodNodeElement setter) {
    this.setter = setter;
  }
  
  @Override
  public Type getConstantType() {
    return constantType;
  }
  
  @Override
  public void setConstantType(Type type) {
    constantType = type;
  }
}
