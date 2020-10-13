#!/usr/bin/env bash
# Convenient script to determine which port should be used in the next test.
# Also checks whether ports are duplicated between tests.
script_path=$(dirname "$0")
port_files=$(grep -Enr '350[0-9]{2}' ${script_path} | \
    sed -En 's/^(.*):[0-9]+:.*(350[0-9]{2}).*$/\2 \1/p' | \
    sort -u)

# Print all used ports and the corresponding files to the user.
echo "${port_files}"

# Detect port duplication.
nb_ports=$(echo "${port_files}" | wc -l)
nb_unique_ports=$(echo "${port_files}" | cut -d ' ' -f1 | sort -u | wc -l)

if [ ${nb_ports} -ne ${nb_unique_ports} ] ; then
    printf "\nSome ports are duplicated!\n"
    duplicated_ports=$(echo "${port_files}" | cut -d ' ' -f1 | uniq -c | sed -En 's/^.*([2-9][0-9]*) +([0-9]{5}).*$/\2/p')
    for port in ${duplicated_ports} ; do
        echo "${port_files}" | grep ${port}
    done
    exit 1
fi

current_max_port=$(echo "${port_files}" | cut -d ' ' -f1 | sort -u | tail -n 1)
printf "\nNext available port is $((current_max_port+1))\n"
