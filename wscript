#!/usr/bin/env python
import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.env["CXXDEFINES"] = ["GL_GLEXT_PROTOTYPES"]
  conf.check_cfg(package='gl', mandatory=True, args='--cflags --libs')
  conf.check_cfg(package='sdl', mandatory=True, args='--cflags --libs')
  conf.check_cfg(package='IL', mandatory=True, args='--cflags --libs')
  
def build(bld):
  webgl = bld.new_task_gen("cxx", "shlib", "node_addon")
  webgl.target = "webgl"
  webgl.source = "src/webgl.cc"
  webgl.uselib = ["GL", "SDL", 'IL']

def shutdown():
  # HACK to get binding.node out of build directory.
  # better way to do this?
  if Options.commands["clean"]:
    if exists("webgl.node"): unlink("webgl.node")
  else:
    if exists("build/default/webgl.node") and not exists("webgl.node"):
      symlink("build/default/webgl.node", "webgl.node")
