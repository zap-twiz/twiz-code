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
        'parser.cc'
      ],
      'dependencies': [
        '../lexer/lexer.gyp:lexer',
      ],
    },
  ],
}
