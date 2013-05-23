{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'chip',
      'type': 'none',
      'sources': [
        'board.h',
        'chip.cc',
        'chip.h',
        'chip_description.h',
        'composite_chip.h',
        'composite_chip_builder.h',
        'fixed_low_chip.h',
        'named_element.h',
        'nand_chip.h',
        'pin.h',
        'thd_work_bench.h',
        'wire.h',
        'work_bench.h'
      ],
      'dependencies': [
      ],
    },
#    {
#      'target_name': 'lexer_test',
#      'type': 'executable',
#      'sources': [
#        'lex_utils_test.cc',
#        'token_stream_test.cc',
#        'token_test.cc',
#      ],
#      'dependencies': [
#        'lexer',
#        '../base/base.gyp:base',
#        '../testing/gtest.gyp:gtestmain',
#        '../testing/gtest.gyp:gtest'
#      ],
#    },
  ],
}
