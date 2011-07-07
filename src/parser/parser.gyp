{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'parser',
      'type': 'executable',
      'sources': [
        'parser.cc',
        'parse_node.cc',
        'parse_node.h',
        'parser_utils.cc',
        'parser_utils.h',
      ],
      'dependencies': [
        '../lexer/lexer.gyp:lexer',
      ],
    },
    {
      'target_name': 'parser_test',
      'type': 'executable',
      'sources': [
        'parser_utils_test.cc',
      ],
      'dependencies': [
        'parser',
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest'
      ],
    },
  ],
}