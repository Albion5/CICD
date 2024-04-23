#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

declare -a tests=(
"FLAG test_case_cat.txt"
)

declare -a extra=(
# "--squeeze-blank test_0_cat.txt"
# "--number-nonblank test_0_cat.txt"
# "--number test_0_cat.txt"
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
    rawstring=$1
    flag=$2
    teststring=${rawstring/FLAG/$flag}
    # test=$(echo "${@/FLAG/$flag}")
    ./../s21_cat $teststring > test_s21_cat.log
    cat $teststring > test_sys_cat.log
    DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
    SDIFF_RES="$(sdiff -s test_s21_cat.log test_sys_cat.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
    then
      (( SUCCESS++ ))
         printf '%3d ' "$COUNTER"
         echo "|SUCCESS| s21_cat $teststring"
    else
      (( FAIL++ ))
         printf '%3d ' "$COUNTER"
         echo "|FAIL| s21_cat $teststring"
         echo "$SDIFF_RES"

    fi
    rm test_s21_cat.log test_sys_cat.log
}


# специфические тесты
for file in "${extra[@]}"
do
    flag="-"
    testing "$file" "$flag"
done

# 1 параметр
for flag1 in b e n s t v
do
    for file in "${tests[@]}"
    do
        flag="-$flag1"
        testing "$file" "$flag"
    done
done

# 2 параметра
for flag1 in b e n s t v
do
    for flag2 in b e n s t v
    do
        if [ $flag1 != $flag2 ]
        then
            for file in "${tests[@]}"
            do
                flag="-$flag1 -$flag2"
                testing "$file" "$flag"
            done
        fi
    done
done

# 3 параметра
for flag1 in b e n s t v
do
    for flag2 in b e n s t v
    do
        for flag3 in b e n s t v
        do
            if [ $flag1 != $flag2 ] && [ $flag2 != $flag3 ] && [ $flag1 != $flag3 ]
            then
                for file in "${tests[@]}"
                do
                    flag="-$flag1 -$flag2 -$flag3"
                    testing "$file" "$flag"
                done
            fi
        done
    done
done

# 2 сдвоенных параметра
for flag1 in b e n s t v
do
    for flag2 in b e n s t v
    do
        if [ $flag1 != $flag2 ]
        then
            for file in "${tests[@]}"
            do
                flag="-$flag1$flag2"
                testing "$file" "$flag"
            done
        fi
    done
done



printf '\n'
echo '--------------------------------------------------------------------'

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
if [ $FAIL -ne 0 ]; then echo "Functional test failed"; exit 1; else echo "Functional test passed"; fi
