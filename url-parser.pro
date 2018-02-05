TEMPLATE=subdirs

SUBDIRS = \
            Engine \   # sub-project names
            UI \
            Tests
   
  # what subproject depends on others
  UI.depends = Engine
  Tests.depends = Engine
  # build the project sequentially as listed in SUBDIRS !
  CONFIG += ordered
