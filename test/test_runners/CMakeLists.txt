add_executable(test all_tests.c
                    test_bus_runner.c
                    test_sharp8080_runner.c
                    ../test_bus.c
                    ../test_sharp8080.c
                    ../unity/unity.c
                    ../unity/unity_fixture.c
                    ${GBC_SOURCE_DIR}/src/bus.c
                    ${GBC_SOURCE_DIR}/src/sharp8080.c)
target_compile_options(test PRIVATE -Werror -Wall -Wextra)
target_compile_definitions(test PRIVATE UNITY_FIXTURE_NO_EXTRAS)