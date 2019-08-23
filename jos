#!/bin/bash
#set -eu

LAB_DIR=/home/cowlog/projects/6.828
LAB_MOUNT_TO=/root/lab
USING_IMAGE="cowlog/jos"
CONTAINER_NAME="jos"
DEFAULT_PORT=10000


function process_options
{
    i=1
    while [ $i -le $# ]; do
	case "${!i}" in
	    -s|--startup) startup=1;;
	    -p|--poweroff) poweroff=1;;
	    -P|--port)
		(( i++ ))
		port=${!i}
		;;
	    -l|--list) desp=1;;
	esac
	(( i++ ))
    done
}

function do_startup
{
    docker run -d -p ${1}:22 -v ${LAB_DIR}:${LAB_MOUNT_TO} --name ${CONTAINER_NAME} ${USING_IMAGE}
}

function do_poweroff
{
    if [ ${1} ]; then
	docker kill ${1}
	docker rm ${1}
    fi
}

function rm_container
{
    ids=$(docker ps -q -f "status=${1}" -f "name=${CONTAINER_NAME}")
    if [ ${ids} ]; then
	docker rm ${ids}
    fi
}

function do_cleanup
{
    rm_container "exited"
    rm_container "created"
}

function do_connect
{
    ssh_port=$(docker port ${CONTAINER_NAME} | grep "22/tcp" | awk -F: '{print $2}')
    # ssh root@localhost -p ${ssh_port}
    docker exec -it ${CONTAINER_NAME} /bin/bash
}

startup=
poweroff=
port=
desp=

if [ ${EUID} != 0 ]; then
    echo "Please run as root"
    exit
fi

process_options $@

container_id=$(docker ps -q -f "name=${CONTAINER_NAME}")


if [ ${poweroff} ]; then
    if [ ${container_id} ]; then
	do_poweroff ${container_id}
    fi

    do_cleanup
    exit
fi

if [ ${desp} ]; then
    if [ ${container_id} ]; then
	echo "container is running!"
	echo "container id: ${container_id}"
	docker port ${CONTAINER_NAME}
	exit
    fi

    if [ $(docker ps -q -f "status=exited" -f "name=${CONTAINER_NAME}") ]; then
	echo "container exited, run: \"sudo jos -p\" to cleanup"
	exit
    fi

    if [ $(docker ps -q -f "status=created" -f "name=${CONTAINER_NAME}") ]; then
	echo "container created, run: \"sudo jos -p\" to cleanup"
	exit
    fi

    echo "no container is running"
    exit
fi

if [ ! ${container_id} ]; then
    do_cleanup
    do_startup ${port:-$DEFAULT_PORT}
fi

if [ ${startup} ]; then
    exit
fi

do_connect
