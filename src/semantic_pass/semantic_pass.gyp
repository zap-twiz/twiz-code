{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'semantic_pass',
      'type': 'executable',
      'sources': [
        'semantic_pass.cc'
      ],
      'dependencies': [
        '../base/base.gyp:base',
        '../chip/chip.gyp:chip',
        '../parser/parser.gyp:parser',
      ],
    },
#    {
#      'target_name': 'semantic_pass_test',
#      'type': 'executable',
#      'sources': [
#      ],
#      'dependencies': [
#        'semantic_pass',
#        '../testing/gtest.gyp:gtestmain',
#        '../testing/gtest.gyp:gtest'
#      ],
#    },
  ],
}
