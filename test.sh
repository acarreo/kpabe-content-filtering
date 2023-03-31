#!/bin/bash

compute_average()
{
  if [ -e "$1" ]; then
    awk '{if ($2 == "params")  {sum += $1; N++}} END {printf "params\t%.2f\n",  (sum/N/1000)}' "$1"
    awk '{if ($2 == "encrypt") {sum += $1; N++}} END {printf "encrypt\t%.2f\n", (sum/N/1000)}' "$1"
    awk '{if ($2 == "dec_key") {sum += $1; N++}} END {printf "dec_key\t%.2f\n", (sum/N/1000)}' "$1"
    awk '{if ($2 == "decrypt") {sum += $1; N++}} END {printf "decrypt\t%.2f\n", (sum/N/1000)}' "$1"
  else
    exit 1
  fi
}

run_test_kpabe()
{
  NB_RANGE="$2"
  TEST_ABE=./test_abe_${1}

  if [ -e "$TEST_ABE" ]; then
    for i in $(seq 1 $NB_RANGE); do
      $TEST_ABE "A5 and ((A1 and A2) or (A3 and A4))" "A1|A2|A4|A5" "www.perdu.com"
      $TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A1|A4" "www.apple.com"
    done > /tmp/result_SAT_${1}

    for i in $(seq 1 $NB_RANGE); do
      $TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A2|A3|A7|A9" "www.github.com"
      $TEST_ABE "(A1 and A2 and A3) or (A4 and A5 and A6)" "A1|A4" "www.microsoft.com"
    done > /tmp/result_not_SAT_${1}

    for i in $(seq 1 $NB_RANGE); do
      $TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A1|A4|A6|A8" "www.youtube.com"
      $TEST_ABE "(A1 and A2) and (A3 or A4)" "A2|A3" "www.twitter.com"
    done > /tmp/result_WL_${1}

    for i in $(seq 1 $NB_RANGE); do
      $TEST_ABE "A5 and ((A1 and A2) or (A3 and A4))" "A1|A2|A4|A5" "www.wikipedia.org"
      $TEST_ABE "(A1 or A2) and (A3 or A4)" "A1|A3" "www.nytimes.com"
    done > /tmp/result_BL_${1}
  else
    exit 1
  fi
}

run_test_kpabe "$1" 100

echo -e "\npolicy satisfied"
compute_average /tmp/result_SAT_${1}

echo -e "\npolicy not satisfied"
compute_average /tmp/result_not_SAT_${1}

echo -e "\nurl whitelisted"
compute_average /tmp/result_WL_${1}

echo -e "\nurl blacklisted"
compute_average /tmp/result_BL_${1}
