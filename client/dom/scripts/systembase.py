#!/usr/bin/python
# Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
# for details. All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

"""This module provides base functionality for systems to generate
Dart APIs from the IDL database."""

import os
import re
from generator import *

class System(object):
  """Generates all the files for one implementation."""

  def __init__(self, templates, database, emitters, output_dir):
    self._templates = templates
    self._database = database
    self._emitters = emitters
    self._output_dir = output_dir
    self._dart_callback_file_paths = []

  def InterfaceGenerator(self,
                         interface,
                         common_prefix,
                         super_interface_name,
                         source_filter):
    """Returns an interface generator for |interface|."""
    return None

  def ProcessCallback(self, interface, info):
    pass

  def GenerateLibraries(self, lib_dir):
    pass

  def Finish(self):
    pass


  def _ProcessCallback(self, interface, info, file_path):
    """Generates a typedef for the callback interface."""
    self._dart_callback_file_paths.append(file_path)
    code = self._emitters.FileEmitter(file_path)

    code.Emit(self._templates.Load('callback.darttemplate'))
    code.Emit('typedef $TYPE $NAME($PARAMS);\n',
              NAME=interface.id,
              TYPE=info.type_name,
              PARAMS=info.ParametersImplementationDeclaration())

  def _GenerateLibFile(self, lib_template, lib_file_path, file_paths,
                       **template_args):
    """Generates a lib file from a template and a list of files.

    Additional keyword arguments are passed to the template.
    """
    # Load template.
    template = self._templates.Load(lib_template)
    # Generate the .lib file.
    lib_file_contents = self._emitters.FileEmitter(lib_file_path)

    # Emit the list of #source directives.
    list_emitter = lib_file_contents.Emit(template, **template_args)
    lib_file_dir = os.path.dirname(lib_file_path)
    for path in sorted(file_paths):
      relpath = os.path.relpath(path, lib_file_dir)
      list_emitter.Emit("#source('$PATH');\n", PATH=relpath)


  def _BaseDefines(self, interface):
    """Returns a set of names (strings) for members defined in a base class.
    """
    def WalkParentChain(interface):
      if interface.parents:
        # Only consider primary parent, secondary parents are not on the
        # implementation class inheritance chain.
        parent = interface.parents[0]
        if IsDartCollectionType(parent.type.id):
          return
        if self._database.HasInterface(parent.type.id):
          parent_interface = self._database.GetInterface(parent.type.id)
          for attr in parent_interface.attributes:
            result.add(attr.id)
          for op in parent_interface.operations:
            result.add(op.id)
          WalkParentChain(parent_interface)

    result = set()
    WalkParentChain(interface)
    return result;


# ------------------------------------------------------------------------------

class InterfacesSystem(System):

  def __init__(self, templates, database, emitters, output_dir):
    super(InterfacesSystem, self).__init__(
        templates, database, emitters, output_dir)
    self._dart_interface_file_paths = []


  def InterfaceGenerator(self,
                         interface,
                         common_prefix,
                         super_interface_name,
                         source_filter):
    """."""
    interface_name = interface.id
    dart_interface_file_path = self._FilePathForDartInterface(interface_name)

    self._dart_interface_file_paths.append(dart_interface_file_path)

    dart_interface_code = self._emitters.FileEmitter(dart_interface_file_path)

    template_file = 'interface_%s.darttemplate' % interface_name
    template = self._templates.TryLoad(template_file)
    if not template:
      template = self._templates.Load('interface.darttemplate')

    return DartInterfaceGenerator(
        interface, dart_interface_code,
        template,
        common_prefix, super_interface_name,
        source_filter)

  def ProcessCallback(self, interface, info):
    """Generates a typedef for the callback interface."""
    interface_name = interface.id
    file_path = self._FilePathForDartInterface(interface_name)
    self._ProcessCallback(interface, info, file_path)

  def GenerateLibraries(self, lib_dir):
    pass


  def _FilePathForDartInterface(self, interface_name):
    """Returns the file path of the Dart interface definition."""
    return os.path.join(self._output_dir, 'src', 'interface',
                        '%s.dart' % interface_name)

# ------------------------------------------------------------------------------

class DartInterfaceGenerator(object):
  """Generates Dart Interface definition for one DOM IDL interface."""

  def __init__(self, interface, emitter, template,
               common_prefix, super_interface, source_filter):
    """Generates Dart code for the given interface.

    Args:
      interface -- an IDLInterface instance. It is assumed that all types have
        been converted to Dart types (e.g. int, String), unless they are in the
        same package as the interface.
      common_prefix -- the prefix for the common library, if any.
      super_interface -- the name of the common interface that this interface
        implements, if any.
      source_filter -- if specified, rewrites the names of any superinterfaces
        that are not from these sources to use the common prefix.
    """
    self._interface = interface
    self._emitter = emitter
    self._template = template
    self._common_prefix = common_prefix
    self._super_interface = super_interface
    self._source_filter = source_filter


  def StartInterface(self):
    if self._super_interface:
      typename = self._super_interface
    else:
      typename = self._interface.id


    extends = []
    suppressed_extends = []

    for parent in self._interface.parents:
      # TODO(vsm): Remove source_filter.
      if MatchSourceFilter(self._source_filter, parent):
        # Parent is a DOM type.
        extends.append(parent.type.id)
      elif '<' in parent.type.id:
        # Parent is a Dart collection type.
        # TODO(vsm): Make this check more robust.
        extends.append(parent.type.id)
      else:
        suppressed_extends.append('%s.%s' %
                                  (self._common_prefix, parent.type.id))

    comment = ' extends'
    extends_str = ''
    if extends:
      extends_str += ' extends ' + ', '.join(extends)
      comment = ','
    if suppressed_extends:
      extends_str += ' /*%s %s */' % (comment, ', '.join(suppressed_extends))

    if typename in interface_factories:
      extends_str += ' default ' + interface_factories[typename]

    # TODO(vsm): Add appropriate package / namespace syntax.
    (self._members_emitter,
     self._top_level_emitter) = self._emitter.Emit(
         self._template + '$!TOP_LEVEL',
         ID=typename,
         EXTENDS=extends_str)

    element_type = MaybeTypedArrayElementType(self._interface)
    if element_type:
      self._members_emitter.Emit(
          '\n'
          '  $CTOR(int length);\n'
          '\n'
          '  $CTOR.fromList(List<$TYPE> list);\n'
          '\n'
          '  $CTOR.fromBuffer(ArrayBuffer buffer);\n',
        CTOR=self._interface.id,
        TYPE=element_type)


  def FinishInterface(self):
    # TODO(vsm): Use typedef if / when that is supported in Dart.
    # Define variant as subtype.
    if (self._super_interface and
        self._interface.id is not self._super_interface):
      consts_emitter = self._top_level_emitter.Emit(
          '\n'
          'interface $NAME extends $BASE {\n'
          '$!CONSTS'
          '}\n',
          NAME=self._interface.id,
          BASE=self._super_interface)
      for const in sorted(self._interface.constants, ConstantOutputOrder):
        self._EmitConstant(consts_emitter, const)

  def AddConstant(self, constant):
    if (not self._super_interface or
        self._interface.id is self._super_interface):
      self._EmitConstant(self._members_emitter, constant)

  def _EmitConstant(self, emitter, constant):
    emitter.Emit('\n  static final $TYPE $NAME = $VALUE;\n',
                 NAME=constant.id,
                 TYPE=constant.type.id,
                 VALUE=constant.value)

  def AddAttribute(self, getter, setter):
    if getter and setter and getter.type.id == setter.type.id:
      self._members_emitter.Emit('\n  $TYPE $NAME;\n',
                                 NAME=getter.id, TYPE=getter.type.id);
      return
    if getter and not setter:
      self._members_emitter.Emit('\n  final $TYPE $NAME;\n',
                                 NAME=getter.id, TYPE=getter.type.id);
      return
    raise Exception('Unexpected getter/setter combination %s %s' %
                    (getter, setter))

  def AddIndexer(self, element_type):
    # Interface inherits all operations from List<element_type>.
    pass

  def AddOperation(self, info):
    """
    Arguments:
      operations - contains the overloads, one or more operations with the same
        name.
    """
    self._members_emitter.Emit('\n'
                               '  $TYPE $NAME($PARAMS);\n',
                               TYPE=info.type_name,
                               NAME=info.name,
                               PARAMS=info.ParametersInterfaceDeclaration())

  # Interfaces get secondary members directly via the superinterfaces.
  def AddSecondaryAttribute(self, interface, getter, setter):
    pass

  def AddSecondaryOperation(self, interface, attr):
    pass

  def AddEventAttributes(self, event_attrs):
    pass

# Given a sorted sequence of type identifiers, return an appropriate type
# name
def TypeName(typeIds, interface):
  # Dynamically type this field for now.
  return 'var'

# ------------------------------------------------------------------------------

class DummyInterfaceGenerator(object):
  """Generates nothing."""

  def __init__(self, system, interface):
    pass

  def StartInterface(self):
    pass

  def FinishInterface(self):
    pass

  def AddConstant(self, constant):
    pass

  def AddAttribute(self, getter, setter):
    pass

  def AddSecondaryAttribute(self, interface, getter, setter):
    pass

  def AddSecondaryOperation(self, interface, info):
    pass

  def AddIndexer(self, element_type):
    pass

  def AddTypedArrayConstructors(self, element_type):
    pass

  def AddOperation(self, info):
    pass

  def AddEventAttributes(self, event_attrs):
    pass
