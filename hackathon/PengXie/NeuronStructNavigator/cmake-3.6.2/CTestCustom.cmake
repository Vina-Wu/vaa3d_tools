list(APPEND CTEST_CUSTOM_ERROR_MATCH
  "ERROR:")

list(APPEND CTEST_CUSTOM_WARNING_EXCEPTION
  "xtree.[0-9]+. : warning C4702: unreachable code"
  "warning LNK4221"
  "warning LNK4204" # Occurs by race condition with objects in small libs
  "variable .var_args[2]*. is used before its value is set"
  "jobserver unavailable"
  "warning: \\(Long double usage is reported only once for each file"
  "warning: To disable this warning use"
  "could not be inlined"
  "libcmcurl.*has no symbols"
  "not sorted slower link editing will result"
  "stl_deque.h:479"
  "Utilities.cmzlib."
  "Utilities.cmbzip2."
  "Source.CTest.Curl"
  "Source.CursesDialog.form"
  "Source.cm_sha2.*warning.*cast increases required alignment of target type"
  "Utilities.cmcurl"
  "Utilities.cmexpat."
  "Utilities.cmlibarchive"
  "warning: declaration of .single. shadows a global declaration"
  "/usr/include.*(warning|note).*shadowed declaration is here"
  "/usr/bin/ld.*warning.*-..*directory.name.*bin.*does not exist"
  "Redeclaration of .send..... with a different storage class specifier"
  "is not used for resolving any symbol"
  "Clock skew detected"
  "remark\\(1209"
  "remark: .*LOOP WAS VECTORIZED"
  "warning .980: wrong number of actual arguments to intrinsic function .std::basic_"
  "LINK : warning LNK4089: all references to.*ADVAPI32.dll.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*CRYPT32.dll.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*PSAPI.DLL.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*RPCRT4.dll.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*SHELL32.dll.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*USER32.dll.*discarded by /OPT:REF"
  "LINK : warning LNK4089: all references to.*ole32.dll.*discarded by /OPT:REF"
  "Warning.*: .*/Utilities/KWIML/test/test_int_format.h.* # Redundant preprocessing concatenation"
  "Warning: library was too large for page size.*"
  "Warning: public.*_archive_.*in module.*archive_*clashes with prior module.*archive_.*"
  "Warning: public.*BZ2_bz.*in module.*bzlib.*clashes with prior module.*bzlib.*"
  "Warning: public.*_archive.*clashes with prior module.*"
  "Warning: LINN32: Last line.*is less.*"
  "Warning: Olimit was exceeded on function.*"
  "Warning: To override Olimit for all functions in file.*"
  "warning.*directory name.*CMake-Xcode.*/bin/.*does not exist.*"
  "stl_deque.h:1051"
  "(Lexer|Parser).*warning.*conversion.*may (alter its value|change the sign)"
  "(Lexer|Parser).*warning.*(statement is unreachable|will never be executed)"
  "PGC-W-0095-Type cast required for this conversion.*ProcessUNIX.c"
  "[Qq]t([Cc]ore|[Gg]ui|[Ww]idgets).*warning.*conversion.*may alter its value"
  "warning:.*is.*very unsafe.*consider using.*"
  "warning:.*is.*misused, please use.*"
  "cmake.version.manifest.*manifest authoring warning.*Unrecognized Element"
  "cc-3968 CC: WARNING File.*" # "implicit" truncation by static_cast
  "ld: warning: directory not found for option .-(F|L)"
  "ld: warning .*/libgcc.a archive's cputype"
  "ld: warning: ignoring file .*/libgcc.a, file was built for archive which is not the architecture being linked"
  "ld: warning: in .*/libgcc.a, file is not of required architecture"
  "warning.*This version of Mac OS X is unsupported"
  "clang.*: warning: argument unused during compilation: .-g"
  "note: in expansion of macro" # diagnostic context note
  "cm(StringCommand|CTestTestHandler)\\.cxx.*warning.*rand.*may return deterministic values"
  "cm(StringCommand|CTestTestHandler)\\.cxx.*warning.*rand.*isn.*t random" # we do not do crypto
  "cm(StringCommand|CTestTestHandler)\\.cxx.*warning.*srand.*seed choices are.*poor" # we do not do crypto

  # Ignore clang's summary warning, assuming prior text has matched some
  # other warning expression:
  "[0-9,]+ warnings? generated."

# scanbuild exceptions
  "char_traits.h:.*: warning: Null pointer argument in call to string length function"
  "stl_construct.h:.*: warning: Forming reference to null pointer"
  ".*stl_uninitialized.h:75:19: warning: Forming reference to null pointer.*"
  ".*stl_vector.h:.*: warning: Returning null reference.*"
  "warning: Value stored to 'yymsg' is never read"
  "warning: Value stored to 'yytoken' is never read"
  "index_encoder.c.241.2. warning: Value stored to .out_start. is never read"
  "index.c.*warning: Access to field.*results in a dereference of a null pointer.*loaded from variable.*"
  "cm_sha2.*warning: Value stored to.*is never read"
  "testProcess.*warning: Dereference of null pointer .loaded from variable .invalidAddress.."
  "liblzma/simple/x86.c:[0-9]+:[0-9]+: warning: The result of the '<<' expression is undefined"
  )

if(NOT "Unix Makefiles" MATCHES "Xcode")
  list(APPEND CTEST_CUSTOM_COVERAGE_EXCLUDE
    "XCode"
    )
endif ()

if(NOT "Unix Makefiles" MATCHES "KDevelop")
  list(APPEND CTEST_CUSTOM_COVERAGE_EXCLUDE
    "Kdevelop"
    )
endif ()

list(APPEND CTEST_CUSTOM_COVERAGE_EXCLUDE
  # Exclude kwsys files from coverage results. They are reported
  # (with better coverage results) on kwsys dashboards...
  "/Source/(cm|kw)sys/"

  # Exclude try_compile sources from coverage results:
  "/CMakeFiles/CMakeTmp/"

  # Exclude Qt source files from coverage results:
  "[A-Za-z]./[Qq]t/qt-.+-opensource-src"
  )

list(APPEND CTEST_CUSTOM_MEMCHECK_IGNORE
  kwsys.testProcess-10 # See Source/kwsys/CTestCustom.cmake.in
  )
