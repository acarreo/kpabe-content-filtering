#!/bin/bash

generate_urls()
{
  for i in $(seq 0 $(expr $1 - 2)); do
    if [ $i -lt 10 ]; then
      echo -n "\"$2_0$i\", "
    else
      echo -n "\"$2_$i\", "
    fi
  done
  echo "\"$2_$(expr $1 - 1)\""
}

generate_attributes()
{
  # Generate attributes for Whitelist and Blacklist
  generate_urls 100 "wl"
  generate_urls 100 "bl"

  # Generate attributes for SAT and not SAT policies
  echo "A_00|A_01|A_02|A_03|A_04|A_05|A_06|A_07|A_08|A_09"
  echo "A_01|A_02|A_03|A_04|A_05"
  echo "A_00|A_01"
  echo "A_02|A_06|A_08"
  echo "A_03|A_05|A_07"
}

generate_policies()
{
  # Generate policies
  echo "A_05 and (A_01 and A_02)"
  echo "A_05 and ((A_01 and A_02) or (A_03 and A_04))"
  echo "(A_00 and A_01) or (A_03 and A_04)"
  echo "(A_07 or A_08) and (A_09 or A_06) and (A_01 or A_02)"
  echo "A_07 and (A_06 or A_05) and (A_04 or A_03)"
}

generate_attributes > attributes.txt
generate_policies > policies.txt
