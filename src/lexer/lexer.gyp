{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'lexer',
      'type': 'static_library',
      'sources': [
        'lex_utils.cc',
        'lex_utils.h',
        'line_counting_stream.h',
        'token.cc',
        'token.h',
        'token_stream.cc',
        'token_stream.h',
      ],
      'dependencies': [
        '../streams/streams.gyp:streams',
      ],
    },
    {
      'target_name': 'lexer_test',
      'type': 'executable',
      'sources': [
        'lex_utils_test.cc',
        'token_stream_test.cc',
        'token_test.cc',
      ],
      'dependencies': [
        'lexer',
        '../base/base.gyp:base',
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest'
      ],
    },
  ],
}
