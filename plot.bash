#!/bin/bash
rm Fig/*
scons -f Scons_flow
vpconvert Fig/*.vpl ppi=600 format=jpg color=y bgcolor=w
rm *.rsf
rm *.vpl
rm *.jpg
