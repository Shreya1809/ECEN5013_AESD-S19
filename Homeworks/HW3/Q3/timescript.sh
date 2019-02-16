#!/bin/bash
# Translate dmesg timestamps to human readable format

 FORMAT="%a %b %d %H:%M:%S %Y"

    now=$(date +%s)
    cputime_line=$(grep -m1 "\.clock" /proc/sched_debug)

    if [[ $cputime_line =~ [^0-9]*([0-9]*).* ]]; then
        cputime=$((BASH_REMATCH[1] / 1000))
    fi

    dmesg | while IFS= read -r line; do
        if [[ $line =~ ^\[\ *([0-9]+)\.[0-9]+\]\ (.*) ]]; then
            stamp=$((now-cputime+BASH_REMATCH[1]))
            echo "[$(date +"${FORMAT}" --date=@${stamp})] ${BASH_REMATCH[2]}"
        else
            echo "$line"
        fi
    done
