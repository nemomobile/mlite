COVERAGE_EXCLUDES = \
    /usr/* \
    */moc_*.cpp \
    */*.moc \
    */qrc_*.cpp \
    /build/* \
    /json/* \
    /tests/* \
    mnotificationmanagerproxy.h \
    mnotificationmanagerproxy.cpp \

coverage.CONFIG = phony
coverage.commands = '\
        @echo "--- coverage: extracting info"$$escape_expand(\\n\\t) \
        lcov -c -o lcov.info -d .$$escape_expand(\\n\\t) \
        @echo "--- coverage: excluding $$join(COVERAGE_EXCLUDES, ", ")"$$escape_expand(\\n\\t) \
        lcov -r lcov.info $$join(COVERAGE_EXCLUDES, "' '", "'", "'") -o lcov.info$$escape_expand(\\n\\t) \
        @echo "--- coverage: generating html"$$escape_expand(\\n\\t) \
        genhtml -o coverage lcov.info --demangle-cpp$$escape_expand(\\n\\t) \
        @echo -e "--- coverage: done\\n\\n\\tfile://$${OUT_PWD}/coverage/index.html\\n" \
        '

QMAKE_EXTRA_TARGETS += coverage
