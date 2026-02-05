#!/bin/bash

###############################################################################
# Here we run tests made it by google test by default. You may comment the
# following command to re-run the script.
make test; exit $?

###############################################################################


TEST_ABE=./examples/kpabe_main

separator () {
  echo -e "\033[90m------------------------------ next test ------------------------------\033[0m"
}

echo -e "\n==================== POLICY SATISFIED – DECRYPTION OK ===================="
$TEST_ABE "A5 and ((A1 and A2) or (A3 and A4))" "A1|A2|A4|A5" "www.perdu.com"
separator
$TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A1|A4" "www.apple.com"

echo -e "\n================= POLICY NOT SATISFIED – DECRYPTION FAIL ================="
$TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A2|A3|A7|A9" "www.github.com"
separator
$TEST_ABE "(A1 and A2 and A3) or (A4 and A5 and A6)" "A1|A4" "www.microsoft.com"

echo -e "\n======================= URL WHITELISTED – ACCESS OK ======================"
$TEST_ABE "A5 and ((A1 and A2) or (A3 and A4))" "A1|A2|A4|A5" "www.wikipedia.org"
separator
$TEST_ABE "(A1 or A2) and (A3 or A4)" "A1|A3" "www.nytimes.com"

echo -e "\n======================= URL BLACKLISTED – ACCESS DENIED =================="
$TEST_ABE "(A1 or A2 or A3) and (A4 or A5)" "A1|A4|A6|A8" "www.youtube.com"
separator
$TEST_ABE "(A1 and A2) and (A3 or A4)" "A2|A3" "www.twitter.com"

