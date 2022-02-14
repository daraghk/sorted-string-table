add_test( MemTableTests.BasicInsertion /Users/daraghking/Documents/Repos/sorted-string-table/build/tests [==[--gtest_filter=MemTableTests.BasicInsertion]==] --gtest_also_run_disabled_tests)
set_tests_properties( MemTableTests.BasicInsertion PROPERTIES WORKING_DIRECTORY /Users/daraghking/Documents/Repos/sorted-string-table/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( tests_TESTS MemTableTests.BasicInsertion)
