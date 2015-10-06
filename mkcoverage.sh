

# TODO check if this
# http://gcovr.com/guide.html
# is a better alternative to lcov

#to avoid Merge mismatch for summaries
# run total clean and  rebuild
 find . -name '*.gcda' | xargs rm -f
 find . -name '*.gcno' | xargs rm -f
 NUMJOBS=${NUMJOBS:--j3}
 make clean && make $NUMJOBS VERBOSE=1

# Run the code so the gcov data files (.gcda) are generated. For example:

make test

# Make empty .gcda files for each existing .gcno file.
# This is so report will show 0.0% for missing tests.
# This step is not needed.

# Removes the old gcov data files (.gcda)
#lcov --directory ./CMakeFiles/sl3.dir/src/sl3 --zerocounters


find . -name "*.gcno" | while read line ; do
  gcda=`echo $line | sed 's/\.gcno$/.gcda/'` ;
  if [ \! -e $gcda ]  ; then
    touch $gcda && echo created empty $gcda;
  fi ;
done

# TODO rename the info suffix to something less common like lcov
rm -f *.lcov


# TODO to avoid later used --remove actions, use a filter in generation, see man lcov

# Generate the lcov trace file.
lcov --capture --directory ./CMakeFiles/sl3.dir/src/sl3 --output-file src.lcov

lcov --capture --directory ./tests/CMakeFiles/sl3_basics.dir/basics --output-file basics.lcov
lcov --capture --directory ./tests/sample/CMakeFiles/sl3_sample.dir --output-file sample.lcov
lcov --capture --directory ./tests/CMakeFiles/test_main.dir --output-file test.lcov



#merge the files
lcov -a src.lcov \
 -a basics.lcov \
 -a sample.lcov \
 -a test.lcov \
 -o all.lcov


# Remove old generate webpages.
rm -rf coverage/



# Remove files not needed in the report.
# Adjust this as needed.
lcov  --remove all.lcov "/usr/*" --output stage1.lcov
lcov  --remove stage1.lcov "*local/opt/*" --output stage2.lcov


# Generate the HTML webpages from the final lcov trace file.
genhtml -o coverage stage2.lcov
