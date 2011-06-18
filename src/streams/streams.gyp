{
  'includes': [
    '../build/common.gypi',
  ],
  'target_defaults': {},
  'targets': [
    {
      'target_name': 'streams',
      'type': 'static_library',
      'sources': [
        'buffered_stream.h',
        'counter.h',
        'iostream.cc',
        'iostream.h',
        'stream_with_observer.h',
        'stream_with_observer_inl.h',
        'nested_stream.h',
        'stream.h',
      ],
    },
    {
      'target_name': 'streams_test',
      'type': 'executable',
      'sources': [
        'buffered_stream_test.cc',
        'iostream_test.cc',
        'nested_stream_test.cc',
        'stream_with_observer_test.cc',
      ],
      'dependencies': [
        'streams',
        '../testing/gtest.gyp:gtestmain',
        '../testing/gtest.gyp:gtest'
      ],
    },
  ],
}
