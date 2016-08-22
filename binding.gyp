{
    'variables': {
        'include_files': [
            'include/nitrogen/nitrogen.hpp',
            'include/nitrogen/aligned_allocator.hpp',
            'include/nitrogen/geometry.hpp',
            'include/nitrogen/nodeutil.hpp',
            'include/nitrogen/threads.hpp',
        ],
        'source_files': [
            'src/nitrogen.cpp',
            'src/nodeutl.cpp',
        ],
        'include_dirs': ['include', '<!(node -e require(\'v8pp\'))'],
    },
    'targets': [
        {
            'target_name': 'nitrogen',
            'cflags_cc+': ['-std=c++11', '-fexceptions'],
            'msvs_settings': { 'VCCLCompilerTool': { 'ExceptionHandling': 1 } },
            'xcode_settings': { 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' },
            'include_dirs': ['<@(include_dirs)'],
            'direct_dependent_settings': {
                'include_dirs': ['<@(include_dirs)'],
            },
            'defines': ['NITROGEN_EXPORTS'],
            'defines!': ['V8_DEPRECATION_WARNINGS=1'],
            'sources': ['<@(include_files)', '<@(source_files)'],
        },
    ],
}