# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../build/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'gtest',
      'type': 'static_library',
      'sources': [
        # Sources based on files in r267 of gmock.
        'gtest/include/gtest/gtest-death-test.h',
        'gtest/include/gtest/gtest-message.h',
        'gtest/include/gtest/gtest-param-test.h',
        'gtest/include/gtest/gtest-spi.h',
        'gtest/include/gtest/gtest-test-part.h',
        'gtest/include/gtest/gtest-typed-test.h',
        'gtest/include/gtest/gtest.h',
        'gtest/include/gtest/gtest_pred_impl.h',
        'gtest/include/gtest/gtest_prod.h',
        'gtest/include/gtest/internal/gtest-death-test-internal.h',
        'gtest/include/gtest/internal/gtest-filepath.h',
        'gtest/include/gtest/internal/gtest-internal.h',
        'gtest/include/gtest/internal/gtest-linked_ptr.h',
        'gtest/include/gtest/internal/gtest-param-util-generated.h',
        'gtest/include/gtest/internal/gtest-param-util.h',
        'gtest/include/gtest/internal/gtest-port.h',
        'gtest/include/gtest/internal/gtest-string.h',
        'gtest/include/gtest/internal/gtest-tuple.h',
        'gtest/include/gtest/internal/gtest-type-util.h',
        'gtest/src/gtest-all.cc',
        'gtest/src/gtest-death-test.cc',
        'gtest/src/gtest-filepath.cc',
        'gtest/src/gtest-internal-inl.h',
        'gtest/src/gtest-port.cc',
        'gtest/src/gtest-test-part.cc',
        'gtest/src/gtest-typed-test.cc',
        'gtest/src/gtest.cc',
      ],
      'sources!': [
        'gtest/src/gtest-all.cc',  # Not needed by our build.
      ],
      'include_dirs': [
        'gtest',
        'gtest/include',
      ],
      'direct_dependent_settings': {
        'defines': [
          'UNIT_TEST',
        ],
        'include_dirs': [
          'gtest/include',  # So that gtest headers can find themselves.
        ],
        'target_conditions': [
          ['_type=="executable"', {'test': 1}],
        ],
        'msvs_disabled_warnings': [4800],
      },
    },
    {
      # Note that calling this "gtest_main" confuses the scons build,
      # which uses "_main" on scons files to produce special behavior.
      'target_name': 'gtestmain',
      'type': 'static_library',
      'dependencies': [
        'gtest',
      ],
      'sources': [
        'gtest/src/gtest_main.cc',
      ],
    },
  ],
}
