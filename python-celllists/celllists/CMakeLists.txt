# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties( celllists.pyx PROPERTIES CYTHON_IS_CXX TRUE )

# Multi-file cython modules do not appear to be working at the moment.
cython_add_module( celllists celllists.pyx )

find_library(CELLLISTS_LIBRARY celllists
             HINTS ENV LD_LIBRARY_PATH)

target_link_libraries(celllists ${CELLLISTS_LIBRARY})
