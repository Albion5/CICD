#!/bin/bash

COUNTER=0

declare -a tests=(
"-s FLAG test_case_cat.txt"
"-s FLAG no_file.txt"
)

declare -a extra=(
"-s -t -n test_0_cat.txt "
"-s -n -e cat_out_bytes.txt"
"-s test_1_cat.txt"
"-b -e -n -s -t -v test_1_cat.txt"
"-t test_3_cat.txt"
"-b -n test_2_cat.txt"
"no_file.txt"
"-b -n test_1_cat.txt"
"-s -n -e test_4_cat.txt"
"-v -n test_1_cat.txt"
"-t -n test_1_cat.txt test_2_cat.txt"
"-v test_5_cat.txt"
)

testing()
{
    # echo "s21_grep $test" >> test_s21_grep.log
    test=$(echo "${@/FLAG/$flag}")
    echo "|$COUNTER| s21_cat $test" >> test_s21_cat.log
    valgrind --quiet --leak-check=full -s --track-origins=yes ./../s21_cat $test 2>> test_s21_cat.log
    printf '\n' >> test_s21_cat.log
    (( COUNTER++ ))
}
rm test_s21_cat.log

# специфические тесты
for file in "${extra[@]}"
do
    flag="-"
    testing $file
done

# 1 параметр
for flag1 in b e n t v
do
    for file in "${tests[@]}"
    do
        flag="-$flag1"
        testing $file
    done
done

# # 2 параметра
# for flag1 in b e n t v
# do
#     for flag2 in b e n t v
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

# 3 параметра
# for flag1 in b e n t v
# do
#     for flag2 in b e n t v
#     do
#         for flag3 in b e n t v
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
# for flag1 in b e n t v
# do
#     for flag2 in b e n t v
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


# # 4 параметра
# for flag1 in b e n t v
# do
#     for flag2 in b e n t v
#     do
#         for flag3 in b e n t v
#         do
#             for flag4 in b e n t v
#             do
#                 if [ $flag1 != $flag2 ] && [ $flag2 != $flag3 ] \
#                 && [ $flag1 != $flag3 ] && [ $flag1 != $flag4 ] \
#                 && [ $flag2 != $flag4 ] && [ $flag3 != $flag4 ]
#                 then
#                     for file in "${tests[@]}"
#                     do
#                         file="-$flag1 -$flag2 -$flag3 -$flag4"
#                         testing $file
#                     done
#                 fi
#             done
#         done
#     done
# done

printf '\n'
echo '--------------------------------------------------------------------'

RESULT="$(grep "ERROR SUMMARY: 0" test_s21_cat.log -c)"
if [ "$COUNTER" == "$RESULT" ]
then
    RESULT="|SUCCESS|"
    
else
    RESULT="|FAIL|"
    printf '\n'
fi

echo "|ALL| $COUNTER $RESULT"