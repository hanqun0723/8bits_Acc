#!/bin/bash

gcc -fsplit-stack gen_data.cpp -o gen_data
./gen_data