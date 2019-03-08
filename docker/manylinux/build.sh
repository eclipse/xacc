#!/bin/bash

for version in cp37-cp37m cp36-cp36m cp35-cp35m cp34-cp34m cp33-cp33m
do
    export ver=`case $version in "cp36-cp36m") echo 3.6 ;; "cp37-cp37m") echo 3.7 ;; "cp35-cp35m") echo 3.5 ;; "cp34-cp34m") echo 3.4 ;; "cp33-cp33m") echo 3.3 ;; "cp27-cp27m") echo 2.7 ;; "cp27-cp27mu") echo 2.7 ;; *) echo "invalid";; esac`
	docker build -t xacc/wheels --build-arg python_version_str=${version} --build-arg python_version=${ver} . --no-cache
    tmpid=$(docker run -d xacc/wheels bash -c "mkdir extract; cd extract ; mv /all_wheels/*.whl /extract ") && sleep 5 && docker cp $tmpid:/extract . && docker rm -v $tmpid
    cd extract && mkdir -p ${version} && mv *.whl ${version}/ && cd ${version}
    testid=$(docker run -d --rm python:3.6 bash -c "tail -f /dev/null") && for wheel in *; do docker cp $wheel $testid:/ ; done && docker exec -it $testid bash -c "python -m pip install *.whl && python -c \"import xacc; xacc.Initialize()\"" && docker stop $testid
    cd ../../
done
