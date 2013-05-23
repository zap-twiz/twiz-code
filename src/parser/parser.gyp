{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'parser',
      'type': 'static_library',
      'sources': [
        'parser.cc',
        'parser.h',
        'parse_error.h',
        'parse_error_collection.h',
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
        'stream_set.cc',
        'stream_set.h',
        'parser_test.cc',
        'parser_utils_test.cc',
      ],
      'dependencies': [
        'parser',
        '../base/base.gyp:base',
        '../lexer/lexer.gyp:lexer',
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest'
      ],
    },
  ],
}
