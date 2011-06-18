{
  'includes': [
    '../build/common.gypi',
  ],
  'targets' : [
    {
      'target_name': 'base',
      'type': 'none',
      'sources': [
        'pointer_traits.h',
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
        'trie_test.cc',
        'trie_node_test.cc',
      ],
      'dependencies': [
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest',
      ],
    }
  ]
}
