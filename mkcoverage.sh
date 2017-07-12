#!/bin/bash

set -e

ME=`basename "$0"`

function printHelp {
cat <<EOF 

Code coverage generation script.

Works only in a build directory configured with coverage information.
Do not run it elsewhere, it might have undefined behaviour.

Options:
  -i or --ignore
    List of files or file patter to ignore
    For example:
      $ME -i "test/* proto/* include/gepard/zmq.hpp"
      or 
      $ME --ignore="test/* proto/* include/gepard/zmq.hpp"

    Patterns are relative to the soure directory

  -a or --append
    List of files additional to execute after running make test
    For example:
      $ME -a "bin/tests/example_first bin/tests/example_second"
      or
      $ME --append="bin/tests/example_first bin/tests/example_second"

    Paths are relative to the current build directory
    
  --branch
    Enable branch coverage. 
    Since this can be time intensive for running and generating the report, 
    this option is disabled per default.
    
    
  --html
    only produce local html report, no Jenkins xml report   

  --xml
    only produce Jenkins xml report, no local html report    

  -h or --help
    Show this help, obviously, and exit  
EOF
}

# do not glob for this
set -f

for i in "$@" ; do

  if [[ $SKIP ]] ; then
    unset SKIP
    continue
  fi

  case $i in
      -i=*|--ignore=*)
      IGNORE="${i#*=}"
      shift 
      ;;

      -i|--ignore)
      shift 
      IGNORE="$1" 
      shift
      SKIP=1 
      ;;

      -a=*|--append=*)
      APPEND="${i#*=}"
      shift 
      ;;

      -a|--append)
      shift 
      APPEND="$1"
      shift
      SKIP=1 
      ;;

      --branch)
      BRANCH_COVERAGE="true"
      shift
      ;;

      --html)
      HTML_ONLY="true"
      shift
      ;;

      --xml)
      XML_ONLY="true"
      shift
      ;;

      -h|--help)
      printHelp
      exit 0
      ;;
      
      
      *)
      echo "unkonow option $i"        
      ;;
  esac
done

#need back my glob
set +f

# to avoid Merge mismatch for summaries run total clean and  rebuild
find . -name '*.gcda' | xargs rm -f
find . -name '*.gcno' | xargs rm -f

# I think we do not want this on jenkings, but deffinitely on my host, so...
NUMJOBS=${NUMJOBS:--j$(nproc)}
make clean
make $NUMJOBS VERBOSE=1

# Run the code so the gcov data files (.gcda) are generated. For example:
make test

for app in ${APPEND[@]} ; do
  $app 
done

# Make empty .gcda files for each existing .gcno file.
# This is so report will show 0.0% for missing tests.
# This step is not needed.
find . -name "*.gcno" | while read line ;
do
  gcda=`echo $line | sed 's/\.gcno$/.gcda/'` ;
  if [ ! -e $gcda ]  ; then
    touch $gcda && echo created empty $gcda;
  fi ;
done

# remove existing stuff lcov
rm -f *.lcov


if [ ${BRANCH_COVERAGE:+1} ] ; then
  BRANCHCOV="--rc lcov_branch_coverage=1"
  echo "enable branch coverage"
else  
  BRANCHCOV=""
fi


merge_stack=""
counter=0
for gcno_dir in $(find tests CMakeFiles -name "*.gcno" | xargs -n1 dirname  | sort -u);
do
  
    # ignore Qt moc shit, if you name your stuff with *_autogen, bad luck
    if [[ "$gcno_dir" == *_autogen ]] ;then
      continue
    fi

    # not interested in sqlte coverage so far
    if [[ "$gcno_dir" == *sqlite ]] ;then
      continue
    fi
    counter=$((counter+1))

    lcov \
            --capture $BRANCHCOV \
            --directory $gcno_dir  \
            --base-directory $(readlink -f ../) \
            --no-external \
            --output-file $counter.lcov

    merge_stack=$merge_stack" -a "$counter.lcov

done

lcov $merge_stack --ignore-errors 'source' -o all.lcov  


# don't glob
set -f


counter=0
cur_file="all.lcov"
nextfile=$cur_file # initialize for the case that ignore is empty 
for exclude in ${IGNORE[@]} ;
do
  echo "HERE AND EXLUDE $exclude"

  counter=$((counter+1))
  nextfile="all_$counter.lcov"

  lcov --remove $cur_file "$exclude"  --output "$nextfile"
  cur_file=$nextfile

done

cp $nextfile project_cov.lcov


rm -f project_testcoverage.xml
rm -rf coverage/

# so lone I do html ony
HTML_ONLY="1"

if ! [ ${HTML_ONLY:+1} ] ; then
  lcov_cobertura-xml project_cov.lcov -o project_testcoverage.xml --demangle
fi

if ! [ ${XML_ONLY:+1} ] ; then
  genhtml -o coverage "project_cov.lcov"
fi




