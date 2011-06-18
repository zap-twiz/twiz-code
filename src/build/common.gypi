# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
  },
  
  'target_defaults': {
    'include_dirs': [
      '..',
    ],
  },
  'conditions': [
    ['OS == "win"',
      {
        'variables': {
        },
        'target_defaults': {
          'defines': [
             'WIN32',
             'UNICODE',
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'WarningLevel': '1',
              'WarnAsError': 'true',
              'DebugInformationFormat': '3',
              'AdditionalOptions': '/MP',
            },
            'VCLinkerTool': {
              'SubSystem': '1', # 1 console
            }
          },
          'configurations': {
            'Debug': {
              'msvs_settings': {
                'VCCLCompilerTool': {
                  'Optimization': '0',    # 0 = /Od
                  'PreprocessorDefinitions': ['_DEBUG', 'DEBUG'],
                  'RuntimeLibrary': '3',  # 3 = /MDd (debug DLL)
                },
                'VCLinkerTool': {
                  'GenerateDebugInformation': 'true',
                },
              },
            },
            'Release': {
              'msvs_settings': {
                'VCCLCompilerTool': {
                  'Optimization': '2',    # 2 = /Os
                  'PreprocessorDefinitions': ['NDEBUG'],
                  'RuntimeLibrary': '2',  # 2 = /MD (nondebug DLL)
                },
                'VCLinkerTool': {
                  'GenerateDebugInformation': 'false',
                },
              },
            },
          },
        },
      },
    ],
  ],
}
