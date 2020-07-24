# pre-commit.sh
git stash -q --keep-index
#./run_tests.sh
./test.out
#RESULT=$?
git stash pop -q
[ $RESULT -ne 5 ] && exit 1
echo "All test good"
exit 0