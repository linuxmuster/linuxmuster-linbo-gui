#!/usr/bin/env bash

# set to empty / comment out for ONLINE mode
#OFFLINE=true

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${DIR}" ]]; then
    DIR="${PWD}"
fi

cd "$(dirname "$0")"

# shellcheck source=fake_cmd_functions.sh
. "./fake_cmd_functions.sh"
# shellcheck source=fake_cmd_create.sh
. "./fake_cmd_create.sh"
# shellcheck source=fake_cmd_upload.sh
. "./fake_cmd_upload.sh"
# shellcheck source=fake_cmd_initcache.sh
. "./fake_cmd_initcache.sh"

cmd="${1}"
if [[ -n "${cmd}" ]]; then
    shift
fi

case "${cmd}" in
    ip)
        ip
        ;;
    netmask)
    	netmask
    	;;
    hostname)
        hostname
        ;;
    cpu)
        cpu
        ;;
    memory)
        memory
        ;;
    mac)
        mac
        ;;
    size)
        size "$@"
        ;;
    battery)
        battery
        ;;
    authenticate)
        authenticate "$@"
        ;;
    create)
        create "$@"
        ;;
    start)
        start "$@"
        ;;
    partition_noformat)
        echo "Partitioning (noformat)..."
        # doesn't use parameters, doesn't output something essential
        exit 0
        ;;
    partition)
        # see above
        echo "Partitioning..."
        sleep 5
        exit 0
        ;;
    preregister)
        preregister "$@"
        ;;
    initcache)
        initcache "$@"
        ;;
    initcache_format)
        initcache "$@"
        ;;
    mountcache)
        mountcache "$@"
        ;;
    readfile)
        readfile "$@"
        ;;
    ready)
        # script is always ready :-)
        exit 0
        ;;
    register)
        register "$@"
        ;;
    sync)
        synconly "$@"
        ;;
    syncstart)
        synconly "$@"
        ;;
    syncr)
        synconly "$@"
        ;;
    synconly)
        synconly "$@"
        ;;
    update)
        update "$@"
        ;;
    upload)
        upload "$@"
        ;;
    version)
        version
        ;;
    writefile)
        writefile "$@"
        ;;
    listimages)
        listimages "$@"
        ;;
    size_cache)
        linbo_size_cache "$@"
	;;
    size_disk)
        linbo_size_disk "$@"
        ;;
    *)
        help
        ;;
esac
