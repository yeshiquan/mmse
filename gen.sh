#!/bin/bash

## usage: sh gen.sh ./libs/client redis_client_http

gsed="/opt/homebrew/bin/gsed"

if [ $# -eq 2 ]
then
    path=$1
    name=$2
    curdir="$(dirname "$BASH_SOURCE")"
    class_name=`echo $name |  ${gsed} -r 's/_(.)/\U\1/g' | ${gsed} -r 's/^([a-z])/\U\1/g'` 

    define_path=`echo $path | ${gsed} -nr 's/[\./]*([a-z\d/]*)/\1/p' | ${gsed} -nr 's/\/*$//p' | ${gsed} -r 's/\//_/g' | tr 'a-z' 'A-Z'`
    define_file=`echo $name | tr 'a-z' 'A-Z'`

    header=$name.h
    header_file=$path/$name.h
    source_file=$path/$name.cpp
    echo "class --- "$class_name
    echo "header --- "$header
    echo "heaer_file --- "$header_file
    echo "source_file --- "$source_file

    cp ./template/class.h.tpl $header_file
    cp ./template/class.cpp.tpl $source_file

    ${gsed} -i -e "s/{{Class}}/$(echo $class_name | ${gsed} -e 's/\\/\\\\/g; s/\//\\\//g; s/&/\\\&/g')/g" $header_file
    ${gsed} -i -e "s/{{header}}/$(echo $header | ${gsed} -e 's/\\/\\\\/g; s/\//\\\//g; s/&/\\\&/g')/g" $source_file
    ${gsed} -i -e "s/{{Class}}/$(echo $class_name | ${gsed} -e 's/\\/\\\\/g; s/\//\\\//g; s/&/\\\&/g')/g" $source_file

    echo "Create Class done."

    exit 0
fi
