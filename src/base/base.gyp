{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults' : {},
  'targets' : [
    {
      'target_name': 'base',
      'type': 'static_library',
      'sources': [
        'base.h', 
        'pointer_traits.h',
        'string_utils.cc',
        'string_utils.h',
        'trie.h',
        'trie_inl.h',
        'trie_node.h',
        'trie_node_inl.h',
      ],
      'dependencies': [],
    },
    {
      'target_name': 'base_test',
      'type': 'executable',
      'sources': [
        'string_utils_test.cc',
        'trie_test.cc',
        'trie_node_test.cc',
      ],
      'dependencies': [
        'base',
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest',
      ],
    }
  ]
}
