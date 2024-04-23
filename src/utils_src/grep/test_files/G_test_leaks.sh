#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

declare -a tests=(
"s letters.txt FLAG"
"o ../s21_grep.c ../s21_grep.h ../Makefile FLAG"
"int ../s21_grep.c FLAG"
"../s21_grep.c -e for -e ^int  ../s21_grep.h ../Makefile FLAG"
"../s21_grep.c -e H -e ^int FLAG door_struct.txt"
"../s21_grep.c -e regex -e ^print  FLAG -f pattern1.txt"
"../Makefile -e while -e void ../s21_grep.c FLAG -f pattern1.txt"
"Alice_in_Wonderland.txt -e alice  FLAG -f pattern1.txt"
"Alice_in_Wonderland.txt -f pattern2.txt FLAG"
)

declare -a extra=(
"-f empty.txt Alice_in_Wonderland.txt"
"if -n gnu.txt maxmin.txt"
"maxmin.txt -n -e ^\}"
"-c -e /\ maxmin.txt"
"-ce ^int maxmin.txt gnu.txt"
"-invh = gnu.txt maxmin.txt"
"d -e"
"-ie INT int.txt"
"maxmin.txt -echar "
"-v maxmin.txt -e main"
"-one ) gnu.txt"
"-l A Alice_in_wonderland.txt"
"rabbit Alice_in_wonderland.txt -i -k"
"-o -e int maxmin.txt"
"-c -e . letters.txt -e '.'"
)

testing()
{
    test=$(echo "${@/FLAG/$flag}")
    leaks -quiet -atExit -- ./../s21_grep $test > test_s21_grep.log
    leak=$(grep -A100000 leaks test_s21_grep.log)
    (( COUNTER++ ))
    if [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
    then
      (( SUCCESS++ ))
        echo "$COUNTER |SUCCESS| s21_grep $test"
    else
      (( FAIL++ ))
        echo "$COUNTER |FAIL| s21_grep $test"
        echo "$leak"
    fi
    rm test_s21_grep.log
}

# специфические тесты
for file in "${extra[@]}"
do
    flag="-"
    testing $file
done

# # 1 параметр
# for flag1 in v c l n h o i
# do
#     for file in "${tests[@]}"
#     do
#         flag="-$flag1"
#         testing $file
#     done
# done

# # 2 параметра
# for flag1 in v c l n h o i
# do
#     for flag2 in v c l n h o i
#     do
#         if [ $flag1 != $flag2 ]
#         then
#             for file in "${tests[@]}"
#             do
#                 flag="-$flag1 -$flag2"
#                 testing $file
#             done
#         fi
#     done
# done

# # 3 параметра
# for flag1 in v c l n h o i
# do
#     for flag2 in v c l n h o i
#     do
#         for flag3 in v c l n h o i
#         do
#             if [ $flag1 != $flag2 ] && [ $flag2 != $flag3 ] && [ $flag1 != $flag3 ]
#             then
#                 for file in "${tests[@]}"
#                 do
#                     flag="-$flag1 -$flag2 -$flag3"
#                     testing $file
#                 done
#             fi
#         done
#     done
# done

# # 2 сдвоенных параметра
# for flag1 in v c l n h o i
# do
#     for flag2 in v c l n h o i
#     do
#         if [ $flag1 != $flag2 ]
#         then
#             for file in "${tests[@]}"
#             do
#                 flag="-$flag1$flag2"
#                 testing $file
#             done
#         fi
#     done
# done

printf '\n'
echo '--------------------------------------------------------------------'
echo "|FAIL|: $FAIL"
echo "|SUCCESS|: $SUCCESS"
echo "|ALL|: $COUNTER"