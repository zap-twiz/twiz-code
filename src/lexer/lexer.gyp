{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults':
    {
      'include_dirs': [
        '..',
      ],
    },
  'targets': [
    {
      'target_name': 'lexer',
      'type': 'static_library',
      'sources': [
        'lex_utils.cc',
        'lex_utils.h',
        'lex.cc',
        'line_counting_stream.h',
        'token.cc',
        'token.h',
        'token_stream.cc',
        'token_stream.h',
        'trie.h',
        'trie_inl.h',
        'trie_node.h',
        'trie_node_inl.h'
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
        'trie_test.cc',
        'trie_node_test.cc',
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
