{
    'includes': ['common.gypi'],
    'variables': {
        'include_files': [
            'include/nitrogen/nitrogen.hpp',
            'include/nitrogen/aligned_allocator.hpp',
            'include/nitrogen/geometry.hpp',
            'include/nitrogen/nodeutil.hpp',
            'include/nitrogen/threads.hpp',
            'include/nitrogen/url.hpp',
        ],
        'source_files': [
            'src/nitrogen.cpp',
            'src/nodeutl.cpp',
            'src/url.cpp',
        ],
        'include_dirs': ['include', '<!(node -e require(\'v8pp\'))'],
    },
    'targets': [
        {
            'target_name': 'nitrogen',
            'include_dirs': ['<@(include_dirs)'],
            'direct_dependent_settings': {
                'include_dirs': ['<@(include_dirs)'],
            },
            'defines': ['NITROGEN_EXPORTS'],
            'sources': ['<@(include_files)', '<@(source_files)'],
        },
    ],
}